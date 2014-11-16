#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <thread>
#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>
#include "include/rpcconfig.h"

#include "cacheserverimpl.hh"
//pthreads
#include <pthread.h>
#include <vector>
#include <map>
#include "include/helpers.hh"


using namespace std;
using namespace xdr;

string GetOwnIP()
{
  //XXX: Add code to return own IP address as a string
  char buffer[256];
  gethostname(buffer, 256);
  return string(buffer);
}

#define HEARTBEAT_INTERVAL 1
//This function sends HB to the masterServer
//once every HEARTBEAT_INTERVAL
void* heartbeat_loop(void * val)
{
  string masterServer((const char*)val);
  auto fd = tcp_connect(masterServer.c_str(), UNIQUE_MASTER_PORT);
  auto client = new srpc_client<api_v1>{fd.release()};

  heartbeat hb;
  hb.nodeIP = GetOwnIP();
  hb.timeStamp = 0;
 
  while (true) {
    client->sendHeartbeat(hb);
    sleep(HEARTBEAT_INTERVAL);
  }
}

int main(int argc, const char *argv[])
{
    /*
    uint128_t digest;
    getMD5Digest("abhay", &digest);
    map<uint128_t, vector<uint8_t>> test;
    if (test.find(digest) == test.end()) {
      cout << "Nothing in map" << endl;
    } else {
      cout << "Value found in map" << endl;
    }*/
    

    if (argc != 2) {
      cout << "Usage: cacheserver MasterIP" << endl;
      return 0;
    }

    pthread_t heartbeat_thread;
    int hb_res = pthread_create(&heartbeat_thread, NULL,
                                heartbeat_loop, (void*)argv[1]);
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

    return 0;
}

