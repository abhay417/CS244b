// -*- C++ -*-
// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#ifndef __XDR_MASTERSERVER_MASTERSERVERIMPL_HH_INCLUDED__
#define __XDR_MASTERSERVER_MASTERSERVERIMPL_HH_INCLUDED__ 1

#include "masterserverdb.h"

#include "include/masterserver.hh"

class api_v1_masterserver {
public:
  using rpc_interface_type = api_v1;

  api_v1_masterserver() : db() { }
  ~api_v1_masterserver() {}

  std::unique_ptr<Result> list(std::unique_ptr<longstring> arg);
  std::unique_ptr<Result> remove(std::unique_ptr<longstring> arg);
  std::unique_ptr<Result> put(std::unique_ptr<kvpair> arg);
  std::unique_ptr<Result> get(std::unique_ptr<longstring> arg);

private:
  bool isPathGood(std::string);
  bool checkParents(std::string);
  MasterServerDB db;
};

#endif // !__XDR_MASTERSERVER_MASTERSERVERIMPL_HH_INCLUDED__
