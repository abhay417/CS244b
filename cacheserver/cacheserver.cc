#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <thread>
#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>
#include "include/rpcconfig.h"

#include "cacheserverimpl.hh"
#include "lrucache.hh"
#include <pthread.h>
#include <vector>
#include <map>
#include "include/helpers.hh"
#include <iostream>
#include <fstream>

using namespace std;
using namespace xdr;


#define HEARTBEAT_INTERVAL 1
//This function sends HB to the masterServer
//once every HEARTBEAT_INTERVAL
void* heartbeatLoop(void* val)
{
  string masterServer((const char*)val);
  auto fd = tcp_connect(masterServer.c_str(), UNIQUE_MASTER_PORT);
  auto client = new srpc_client<api_v1>{fd.get()};

  heartbeat hb;
  hb.nodeIP = getOwnAddress();
  hb.timeStamp = 0;
 
  while (true) {
    client->sendHeartbeat(hb);
    sleep(HEARTBEAT_INTERVAL);
  }

  fd.clear();
  delete client;
}

int main(int argc, const char *argv[])
{
  /*//HTTP client test
  int headSize;
  httpclient webclient("i.imgur.com");
  vector<uint8_t> httpContent = webclient.sendRequest("/R6u2kaI.png",
                                                      headSize);
  cout << "Headsize" << headSize << endl;
  //print the header
  for (int i = 0; i < headSize; i++) {
    cout << (char) httpContent[i];
  }
  //create a test file for the content
  ofstream myFile;
  myFile.open("testFile");
  for (int i = headSize; i < httpContent.size(); i++) {
    myFile << httpContent[i];
  }
  myFile.close();*/
  

  /*Testing uint128_t conversions
  uint64_t low = 0xFEDCBA;
  uint64_t high = 0xABCDEF;
  uint128_t comb = combineLowHigh(low, high);
  printUint128(comb);
  uint64_t low2 = (uint64_t) comb;
  uint64_t high2 = (uint64_t) (comb >> 64);
  uint128_t comb2 = combineLowHigh(low2, high2);
  printUint128(comb2);
  */

  /*
  Testing cacheStore map
  int128_t digest;
  getMD5Digest("abhay", &digest);
  map<uint128_t, vector<uint8_t>> test;
  if (test.find(digest) == test.end()) {
    cout << "Nothing in map" << endl;
  } else {
    cout << "Value found in map" << endl;
  }
  */
    
  /* LRU test code that I just stuck in this main function
   * because I'm lazy. */
  /*lru_cache<int> cache(3);
  cout << cache << endl;
  cache.put(1, 1);
  cout << cache << endl;
  cache.put(2, 2);
  cout << cache << endl;
  cache.put(3, 3);
  cout << cache << endl;
  cache.put(4, 4);
  cout << cache << endl;
  cache.put(5, 5);
  cout << cache << endl;
  cache.get(3);
  cout << cache << endl;
  cache.get(5);
  cout << cache << endl;
  cache.put(6, 6);
  cout << cache << endl;
  cache.erase(5);
  cout << cache << endl;

  for (auto it = cache.begin(); it != cache.end(); it++) {
    cout << (int) (it->digest) << endl;
  }
  
  return 0;*/

/*  httpclient statsclient("localhost", UNIQUE_STATSSERVER_PORT);
  int statsHeadSize;
  statsclient.sendRequest("/statsServer?q=cacheMiss", statsHeadSize);*/

  if (argc != 2) {
    cout << "Usage: cacheserver MasterIP" << endl;
    return 0;
  }

  pthread_t heartbeatThread;
  int hb_res = pthread_create(&heartbeatThread, NULL,
                              heartbeatLoop, (void*)argv[1]);
  if (hb_res) {
    cout << "Failed to create HB thread: Ret code: " << hb_res << endl;
  } 

  cache_api_v1_server s; 
  rpc_tcp_listener rl(tcp_listen(UNIQUE_CACHESERVER_PORT, AF_INET));

  try {
      rl.register_service(s);
      rl.run();
  } catch (exception &e) {
      cerr << e.what() << endl;
  }

  pthread_join(heartbeatThread, NULL);
  return 0;
}

