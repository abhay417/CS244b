// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include "cacheserverimpl.hh"

//std::unique_ptr<bytestream>
std::unique_ptr<longstring>
cache_api_v1_server::getCacheContents(std::unique_ptr<longstring> arg)
{
  std::string url = *arg;
  //std::unique_ptr<bytestream> ret(new bytestream);
  std::unique_ptr<longstring> ret(new longstring);

  *ret = url + "test";
  return ret;
}

