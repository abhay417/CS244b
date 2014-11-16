// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include "cacheserverimpl.hh"

using namespace std;

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

void
cache_api_v1_server::newCacheserverAdded(std::unique_ptr<newCacheServerInfo> arg)
{
  uint128_t digestFrom = combineLowHigh(arg->fromLow, arg->fromHigh);
  uint128_t digestTo = combineLowHigh(arg->toLow, arg->toHigh); 
  string newServer = arg->newNodeIP;

  //Create connection to the newServer and transfer relevant contents
  cout << "Sending cached data to " << newServer << endl; 
   
}

void
cache_api_v1_server::sendCachedData(std::unique_ptr<cacheTransfer> arg)
{
  uint128_t digest = combineLowHigh(arg->lowDigest, arg->highDigest);
  _cacheStore[digest] = arg->cacheData;

  cout << "Received cached data from " << arg->sourceNodeIP << endl;
  return;
}
