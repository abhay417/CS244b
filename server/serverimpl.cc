// Scaffolding originally generated from include/server.x.
// Edit to add functionality.
 
#include "server/serverimpl.hh"
#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>
#include "include/rpcconfig.h"
#include "include/httpclient.hh"

static vector<uint128_t>
getVirtualNodeDigests(const string& ip)
{
  vector<uint128_t> digestVector;
  uint128_t digest;
  for (int i = 0; i < NUM_VIRTUAL_NODES; i++) {
    string hashVal = ip + VirtualNodeSuffix[i];
    getMD5Digest(hashVal, &digest);
    digestVector.push_back(digest);
  }
  return digestVector;
}

string
api_v1_server::getCacheServer(uint128_t digest)
{
  auto i = _ring.lower_bound(digest);
  if (i == _ring.end()) {
    //wrap around
    i = _ring.begin();
  }
  return i->second;
}

void
api_v1_server::removeTimedOutServers(uint128_t curr_nsec)
{
  vector<string> serversToRemove;  
  for(auto i = _currServers.begin(); i != _currServers.end(); i++) {
    uint128_t timeout_nsec = i->second + HASHRING_TIMEOUT;
    if (timeout_nsec < curr_nsec) {
      serversToRemove.push_back(i->first);
    }
  }

  for (int i = 0; i < serversToRemove.size(); i++) {
    string currServer = serversToRemove[i];
    _currServers.erase(currServer);

    vector<uint128_t> nodeDigests = getVirtualNodeDigests(currServer);
    for (int j = 0; j < nodeDigests.size(); j++) {
        _ring.erase(nodeDigests[j]);
    }
    if (USE_STATSSERVER) {
        httpclient statsclient(STATSSERVER_IP, UNIQUE_STATSSERVER_PORT);
        int statsHeadSize;
        statsclient.sendRequest("/statsServer?q=dropMember", statsHeadSize);
    }

    cout << "removed server " << currServer << " from ring " << endl;
  }
}

void
api_v1_server::sendHeartbeat(std::unique_ptr<heartbeat> arg)
{
  //First get the IP of the node
  string ip = arg->nodeIP;

  //Get the time
  uint128_t curr_nsec = getMonotonicNsec();
  
  //Check if the cache server is already part of the ring
  if (_currServers.find(ip) != _currServers.end()) {
    //This node is already in the ring
    //Update the timestamp of the heatbeat
    _currServers[ip] = curr_nsec;
    //cout << "Server '" << ip << "' already member of the ring. " << 
    //        "Only updating timestamp" << endl;
    removeTimedOutServers(curr_nsec);
    return;
  }

  if (USE_STATSSERVER) {
    httpclient statsclient(STATSSERVER_IP, UNIQUE_STATSSERVER_PORT);
    int statsHeadSize;
    statsclient.sendRequest("/statsServer?q=addMember", statsHeadSize);
  }

  //Add the cache server to the ring
  vector<uint128_t> nodeDigests = getVirtualNodeDigests(ip);
  for (int i = 0; i < nodeDigests.size(); i++) {
    _ring[nodeDigests[i]] = ip;
    cout << "Added virtual node of server '" << ip
         << "' to ring." << endl;
    //printUint128(nodeDigests[i]);
  }
  
  //And add it to the _currentServers
  _currServers[ip] = curr_nsec;
  cout << "Adding timestamp for server '" << ip << "'" << endl;

  //Remove all servers that we have not heard back from
  removeTimedOutServers(curr_nsec);
  
  //Send cacheTransfer message to the other servers
  for (int i = 0; i < nodeDigests.size(); i++) {
    //Create the cacheServerInfo
    uint128_t fromDigest = nodeDigests[i];
    auto nextServerInRing = _ring.find(fromDigest);
    nextServerInRing++;

    
    if (nextServerInRing == _ring.end() ||
        nextServerInRing->second == ip) {
      continue;
    }

    cout << "New server: " << ip << " Next server: "<< nextServerInRing->second << endl;

    uint128_t toDigest = nextServerInRing->first;
    newCacheServerInfo nsInfo;
    nsInfo.fromLow = (uint64_t) fromDigest;
    nsInfo.fromHigh = fromDigest >> 64;
    nsInfo.toLow = (uint64_t)toDigest;
    nsInfo.toHigh = toDigest >> 64;
    nsInfo.newNodeIP = ip;
    
    
    //Create the client 
    auto fd = tcp_connect(nextServerInRing->second.c_str(),
                          UNIQUE_CACHESERVER_PORT);
    auto cclient = new srpc_client<cache_api_v1>{fd.release()};
    cclient->newCacheserverAdded(nsInfo);
  }

  cout<<"Printing ring" << endl;
  for(auto i = _ring.begin(); i != _ring.end(); i++) {
    printUint128(i->first);
    cout << i->second << endl;
  }
}
  

std::unique_ptr<longstring>
api_v1_server::getCacheServer(std::unique_ptr<longstring> arg)
{
  std::string url = *arg;
  std::unique_ptr<longstring> ret(new longstring);
  
  uint128_t digest;
  getMD5Digest(url, &digest);

  if (USE_CHASHING) {
    *ret = getCacheServer(digest);
  } else {
    int i = digest % _currServers.size();
    for (auto server_pair : _currServers) {
      if (i == 0) {
         *ret = server_pair.first;
      } 
      i--;
    }
  }

  cout << "Returning cache server: " << *ret << endl;
  return ret;
}
