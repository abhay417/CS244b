// -*- C++ -*-
// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#ifndef __XDR_SERVER_CACHESERVER_HH_INCLUDED__
#define __XDR_SERVER_CACHESERVER_HH_INCLUDED__ 1

#include "include/server.hh"
#include "lrucache.hh"

class cache_api_v1_server {
public:
  using rpc_interface_type = cache_api_v1;

  std::unique_ptr<cache_data> getCacheContents(std::unique_ptr<longstring> arg);
private: 
  lru_cache<cache_data> cache;
};

#endif // !__XDR_SERVER_CACHESERVERL_HH_INCLUDED__
