// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include "server/serverimpl.hh"


static void
getMD5Digest(const string& str, uint128_t* digest) 
{
  MD5(reinterpret_cast<const unsigned char *>(str.c_str()),
      str.length(),
      reinterpret_cast<unsigned char *>(digest));
}

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

static uint128_t
getMonotonicNsec() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  uint128_t nsec = ts.tv_sec * 1000000000 + ts.tv_nsec;
  return nsec;
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
    
    cout << (uint64_t) (timeout_nsec > 64)
         << (uint64_t) timeout_nsec << " "
         << (uint64_t) (curr_nsec > 64)
         << (uint64_t) curr_nsec << endl;
  }

  for (int i = 0; i < serversToRemove.size(); i++) {
    string currServer = serversToRemove[i];
    _currServers.erase(currServer);

    vector<uint128_t> nodeDigests = getVirtualNodeDigests(currServer);
    for (int j = 0; j < nodeDigests.size(); j++) {
        _ring.erase(nodeDigests[j]);
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
    cout << "Server already member of the ring." << 
            "Only updating timestamp" << endl;
    removeTimedOutServers(curr_nsec);
    return;
  }

  //Add the cache server to the ring
  vector<uint128_t> nodeDigests = getVirtualNodeDigests(ip);
  for (int i = 0; i < nodeDigests.size(); i++) {
    _ring[nodeDigests[i]] = ip;
    cout << "Added server to ring." << endl;
  }
  
  //And add it to the _currentServers
  _currServers[ip] = curr_nsec;
  cout << "Adding server timestamp" << endl;

  //Remove all servers that we have not heard back from
  removeTimedOutServers(curr_nsec);
}
  

std::unique_ptr<longstring>
api_v1_server::getCacheServer(std::unique_ptr<longstring> arg)
{
  std::string url = *arg;
  std::unique_ptr<longstring> ret(new longstring);
  
  uint128_t digest;
  getMD5Digest(url, &digest);

  auto i = _ring.lower_bound(digest);
  if (i == _ring.end()) {
    //wrap around
    i = _ring.begin();
  }

  *ret = i->second;
  cout << "Returning cache server: " << *ret << endl;
  return ret;
}
