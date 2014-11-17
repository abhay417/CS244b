// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include "cacheserverimpl.hh"
#include "include/server.hh"
#include <unistd.h>
#include <sys/socket.h>
#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>

using namespace std;
using namespace xdr;

std::unique_ptr<bytestream>
cache_api_v1_server::getCacheContents(std::unique_ptr<longstring> arg)
{
  string url = *arg;
  cout << "getCacheContents " << url << endl;
  //First check if the URL content is already in our cache
  uint128_t urlDigest;
  getMD5Digest(url, &urlDigest);
  if (_cacheStore.find(urlDigest) == _cacheStore.end()) {
    cout << "Content not in cache so fetching from the origin server" << endl;
    //URL content is not already cached
    //Make an HTTP request to get it, then cache and return it
    size_t firstSlashInd = url.find("/");
    string host = url.substr(0, firstSlashInd);
    string querystr = url.substr(firstSlashInd);
    cout << "host: " << host << " querystr: " << querystr << endl;
    httpclient webclient;
    vector<uint8_t> httpContent = webclient.sendRequest(host, querystr);

    //Cache it
    _cacheStore[urlDigest] = httpContent;
  }

  //Return the content
  //XXX: Doing a memcpy here will probably be faster
  unique_ptr<bytestream> ret(new bytestream);
  vector<uint8_t>& data = _cacheStore[urlDigest];
  ret->resize(data.size());
  for (int i = 0; i < data.size(); i++) {
    ret->push_back(data[i]);
  }  
  
  return ret;
}

/*
XXX: Need to merge lrucache code with the _cacheStore

#include "include/httpclient.hh"
#include "include/server.hh"
#include "lrucache.hh"

#include <iostream>

//std::unique_ptr<bytestream>
std::unique_ptr<cache_data>
cache_api_v1_server::getCacheContents(std::unique_ptr<longstring> arg)
{
  std::string url = *arg;
  std::unique_ptr<cache_data> ret(new longstring);
  //std::string querystr = url.substr(url.find("/"));
  //std::string host = url.substr(0,url.find("/"));
  
  if (cache.contains(url)) {
    std::cout << "HIT: " << url << std::endl;
    *ret = cache.get(url);
  } else {
    std::cout << "MISS: " << url << std::endl;
    httpclient webclient;
    *ret = webclient.sendRequest("www.imgur.com", url);
    cache.put(url, *ret);
  }
  return ret;
}
*/

void
cache_api_v1_server::newCacheserverAdded(std::unique_ptr<newCacheServerInfo> arg)
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
  for (auto i = _cacheStore.begin(); i != _cacheStore.end(); i++) {
    if (i->first >= digestFrom && i->first < digestTo) {
      digestsToTransfer.push_back(i->first);
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
  auto cclient = new srpc_client<cache_api_v1>{fd.release()};
  for (int i = 0; i < digestsToTransfer.size(); i++) {
    uint128_t digest = digestsToTransfer[i];
    vector<uint8_t>& cachedData = _cacheStore[digest];
    cacheTransfer ct;
    ct.lowDigest = digest;
    ct.highDigest = digest >> 64;
    ct.sourceNodeIP = getOwnAddress();
    ct.cacheData.resize(cachedData.size());
    for (int j = 0; j < cachedData.size(); j++) {
      ct.cacheData.push_back(cachedData[j]);
    }  
    cclient->sendCachedData(ct);
    cout << "Cached data sent to " << newServer << endl;
  }

  //Remove the sent cached contents from _cacheStore
  for (int i = 0; i < digestsToTransfer.size(); i++) {
    _cacheStore.erase(digestsToTransfer[i]);
  }
   
}

void
cache_api_v1_server::sendCachedData(std::unique_ptr<cacheTransfer> arg)
{
  uint128_t digest = combineLowHigh(arg->lowDigest, arg->highDigest);
  _cacheStore[digest] = arg->cacheData;

  cout << "Received cached data from " << arg->sourceNodeIP << endl;
  return;
}
