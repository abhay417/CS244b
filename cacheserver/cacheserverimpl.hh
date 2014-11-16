// -*- C++ -*-
// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#ifndef __XDR_SERVER_CACHESERVER_HH_INCLUDED__
#define __XDR_SERVER_CACHESERVER_HH_INCLUDED__ 1

#include "include/server.hh"
#include "include/helpers.hh"
#include "include/httpclient.hh"
#include "lrucache.hh"

#include <map>
=======

class cache_api_v1_server {
private:
  map<uint128_t, vector<uint8_t>> _cacheStore;
  lru_cache<cache_data> cache;
    
public:
  using rpc_interface_type = cache_api_v1;

  std::unique_ptr<bytestream> getCacheContents(std::unique_ptr<longstring> arg);
  void newCacheserverAdded(std::unique_ptr<newCacheServerInfo> arg);
  void sendCachedData(std::unique_ptr<cacheTransfer> arg);
};

#endif // !__XDR_SERVER_CACHESERVERL_HH_INCLUDED__
