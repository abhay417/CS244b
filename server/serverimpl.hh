// -*- C++ -*-
// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#ifndef __XDR_SERVER_SERVERIMPL_HH_INCLUDED__
#define __XDR_SERVER_SERVERIMPL_HH_INCLUDED__ 1

#include "serverdb.h"

#include "include/server.hh"

class api_v1_server {
public:
  using rpc_interface_type = api_v1;

  api_v1_server() : db("kvstore.db") { }
  ~api_v1_server() {}

  std::unique_ptr<Result> create(std::unique_ptr<kvpair> arg);
  std::unique_ptr<Result> remove(std::unique_ptr<longstring> arg);
  std::unique_ptr<Result> set(std::unique_ptr<kvpair> arg);
  std::unique_ptr<Result> list(std::unique_ptr<longstring> arg);
  std::unique_ptr<Result> get(std::unique_ptr<longstring> arg);

private:
  bool isPathGood(std::string);
  bool checkParents(std::string);
  ServerDB db;
};

#endif // !__XDR_SERVER_SERVERIMPL_HH_INCLUDED__
