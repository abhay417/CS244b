// -*- C++ -*-
// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#ifndef __XDR_CACHESERVER_CACHESERVERIMPL_HH_INCLUDED__
#define __XDR_CACHESERVER_CACHESERVERIMPL_HH_INCLUDED__ 1

#include "serverdb.h"

#include "include/cacheserver.hh"

class api_v1_cacheserver {
public:
  using rpc_interface_type = api_v1;

  api_v1_cacheserver() : db() { }
  ~api_v1_cacheserver() {}

  std::unique_ptr<Result> list(std::unique_ptr<longstring> arg);
  std::unique_ptr<Result> find(std::unique_ptr<longstring> arg);
  std::unique_ptr<Result> get(std::unique_ptr<longstring> arg);

private:
  bool isPathGood(std::string);
  bool checkParents(std::string);
  ServerDB db;
};

#endif // !__XDR_CACHESERVER_CACHESERVERIMPL_HH_INCLUDED__
