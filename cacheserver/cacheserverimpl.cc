// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include "cacheserverimpl.hh"
#include "include/server.hh"
#include "include/rpcconfig.h"
#include "lrucache.hh"
#include <unistd.h>
#include <sys/socket.h>
#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>

using namespace std;
using namespace xdr;

static void
removeHttpPrefix(string& url)
{
  string httpPrefix("http://");
  if (strcasecmp(url.substr(0, httpPrefix.length()).c_str(),
                 httpPrefix.c_str()) == 0) {
    //Prefix matched
    url = url.substr(httpPrefix.length());
  }
}


//XXX: Consolidate the common code in the following two functions
unique_ptr<bytestream>
cache_api_v1_server::getCacheContents(unique_ptr<longstring> arg)
{
  string url = *arg;
  
  //First check if the URL content is already in our cache
  uint128_t urlDigest;
  getMD5Digest(url, &urlDigest);
  httpclient statsclient(STATSSERVER_IP, UNIQUE_STATSSERVER_PORT);
  if (!cache.contains(urlDigest)) {
    cout << "Content not in cache so fetching from the origin server" << endl;
    
    //Remove http:// prefix if it was passed
    removeHttpPrefix(url);
    
    //URL content is not already cached
    //Make an HTTP request to get it, then cache and return it
    size_t firstSlashInd = url.find("/");
    string host, querystr;
    if (firstSlashInd == string::npos) {
      host = url;
      querystr = "/";
    } else {
      host = url.substr(0, firstSlashInd);
      querystr = url.substr(firstSlashInd);
    }
    
    cout << "host: " << host << " querystr: " << querystr << endl;
    httpclient webclient(host);
    int headSize;
    vector<uint8_t> httpContent = webclient.sendRequest(querystr, headSize);
    if (USE_STATSSERVER) {
      int statsHeadSize;
      statsclient.sendRequest("/statsServer?q=cacheMiss", statsHeadSize);
    }
    //Cache it
    cache.put(urlDigest, httpContent);
  } else {
    if (USE_STATSSERVER) {
      int statsHeadSize;
      statsclient.sendRequest("/statsServer?q=cacheHit", statsHeadSize);
    }
  }

  //Return the content
  //XXX: Doing a memcpy here will probably be faster
  unique_ptr<bytestream> ret(new bytestream);
  vector<uint8_t>& data = cache.get(urlDigest);
  ret->reserve(data.size());
  for (int i = 0; i < data.size(); i++) {
    ret->push_back(data[i]);
  }

  cout << "Return data size: " << ret->size() << endl;
  return ret;
}

unique_ptr<bytestream>
cache_api_v1_server::getCacheContents2(unique_ptr<cacheRequest> arg)
{
  string host = arg->host;
  string url  = arg->requestUrl;
  bool isHeadRequest = arg->isHeadRequest;
  string request = arg->request;

  cout << "Requesting " << url << endl;

  //First check if the URL content is already in our cache
  uint128_t urlDigest;
  getMD5Digest(url, &urlDigest);
  if (!cache.contains(urlDigest)) {
    cout << "MISS" << endl;
    //URL content is not already cached
    //Make an HTTP request to get it, then cache and return it
    httpclient webclient(host);
    int headSize;
    vector<uint8_t> httpContent = webclient.sendRequest2(request, 
                                                         headSize,
                                                         isHeadRequest);
    if (USE_STATSSERVER) {
      int statsHeadSize;
      httpclient statsclient(STATSSERVER_IP, UNIQUE_STATSSERVER_PORT);
      statsclient.sendRequest("/statsServer?q=cacheMiss", statsHeadSize);
    }
    //Cache it
    cache.put(urlDigest, httpContent);
  } else {
    cout << "HIT" << endl;
    if (USE_STATSSERVER) {
      int statsHeadSize;
      httpclient statsclient(STATSSERVER_IP, UNIQUE_STATSSERVER_PORT);
      statsclient.sendRequest("/statsServer?q=cacheHit", statsHeadSize);
    }
  }

  //Return the content
  //XXX: Doing a memcpy here will probably be faster
  unique_ptr<bytestream> ret(new bytestream);
  vector<uint8_t>& data = cache.get(urlDigest);
  ret->reserve(data.size());
  for (int i = 0; i < data.size(); i++) {
    ret->push_back(data[i]);
  }  

  cout << "Return data size: " << ret->size() << endl << endl;
  return ret;
}

void
cache_api_v1_server::newCacheserverAdded(unique_ptr<newCacheServerInfo> arg)
{
  uint128_t digestFrom = combineLowHigh(arg->fromLow, arg->fromHigh);
  uint128_t digestTo = combineLowHigh(arg->toLow, arg->toHigh); 
  string newServer = arg->newNodeIP;

  //printUint128(digestFrom);
  //printUint128(digestTo);
  //cout << "'digestFrom <= digestTo': " << (digestFrom <= digestTo) << endl; 

  //Create connection to the newServer and transfer relevant contents
  cout << "Sending cached data to " << newServer << endl;

  //Get all the digest values that need to be sent
  vector<uint128_t> digestsToTransfer;
  for (auto i = cache.begin(); i != cache.end(); i++) {
    if (i->digest >= digestFrom && i->digest < digestTo) {
      digestsToTransfer.push_back(i->digest);
    }
  }

  if (digestsToTransfer.empty()) {
    cout << "No cached data to send to " << newServer << endl;
    return;
  }
  
  //cout << "Num cached data to transfer to " << newServer << ": "
  //     << digestsToTransfer.size() << endl;

  //Transfer the contents
  auto fd = tcp_connect(newServer.c_str(), UNIQUE_CACHESERVER_PORT);
  auto cclient = new srpc_client<cache_api_v1>{fd.get()};
  for (int i = 0; i < digestsToTransfer.size(); i++) {
    uint128_t digest = digestsToTransfer[i];
    vector<uint8_t>& cachedData = cache.get(digest);
    cacheTransfer ct;
    ct.lowDigest = digest;
    ct.highDigest = digest >> 64;
    ct.sourceNodeIP = getOwnAddress();
    ct.cacheData.reserve(cachedData.size());
    for (int j = 0; j < cachedData.size(); j++) {
      ct.cacheData.push_back(cachedData[j]);
    }  
    cclient->sendCachedData(ct);
    cout << "Cached data sent to " << newServer << endl;
  }

  //Remove the sent cached contents from _cacheStore
  for (int i = 0; i < digestsToTransfer.size(); i++) {
    cache.erase(digestsToTransfer[i]);
  }
   
  fd.clear();
  delete cclient;
}

void
cache_api_v1_server::sendCachedData(unique_ptr<cacheTransfer> arg)
{
  uint128_t digest = combineLowHigh(arg->lowDigest, arg->highDigest);
  cache.put(digest, arg->cacheData);

  cout << "Received cached data from " << arg->sourceNodeIP << endl;
  return;
}
