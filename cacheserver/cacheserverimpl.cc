// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include "cacheserverimpl.hh"
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

