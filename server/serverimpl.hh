// -*- C++ -*-
// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#ifndef __XDR_SERVER_SERVERIMPL_HH_INCLUDED__
#define __XDR_SERVER_SERVERIMPL_HH_INCLUDED__ 1

#include "include/server.hh"
#include "include/helpers.hh"

#include <string>
#include <map>
#include <iostream>

using namespace std;
using namespace xdr;

//Proxy server loop with be declared a friend function to the
//class api_v1_server so we can access the HashRing '_ring' 
extern void*
proxyServerWorkerLoop(void* dummy);

#define HASHRING_TIMEOUT 3000000000
#define NUM_VIRTUAL_NODES 4
//Note: The number of suffixes in the following
//      array needs to be greater than or equal
//      to NUM_VIRTUAL_NODES
const char * const VirtualNodeSuffix[] = {
  "node1", "node2", "node3", "node4"
};

typedef map<uint128_t, string> Hashring;

class api_v1_server {
private:
  Hashring _ring;
  map<string, uint128_t> _currServers;

  string getCacheServer(uint128_t digest) const;
  void removeTimedOutServers(uint128_t current_nsec);
  friend void* proxyServerWorkerLoop(void*);
public:
  using rpc_interface_type = api_v1;

  void sendHeartbeat(std::unique_ptr<heartbeat> arg);
  std::unique_ptr<longstring> getCacheServer(std::unique_ptr<longstring> arg);
};

#endif // !__XDR_SERVER_SERVERIMPL_HH_INCLUDED__
