
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <set>
#include <string>

#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>

#include <include/rpcconfig.h>
#include <include/server.hh>
#include <include/client.h>

using namespace std;
using namespace xdr;

Client::Client()
{
    client = NULL;
}

Client::~Client()
{
    this->close();
}

void
Client::open(const std::string &host)
{
    if (isConnected()) {
        cout << "Already connected!" << endl;
        exit(1);
    }

    auto fd = tcp_connect(host.c_str(), UNIQUE_MASTER_PORT);
    client = new srpc_client<api_v1>{fd.release()};
}

void
Client::close()
{
    if (isConnected()) {
        delete client;
        client = NULL;
    }
}

bool
Client::isConnected()
{
    return client != NULL;
}

void
Client::heartbeat(const string& ip)
{
  //XXX: modify this to send own IP address

  struct heartbeat hb;
  hb.nodeIP = ip;
  hb.timeStamp = 0;

  client->sendHeartbeat(hb);
}

string
Client::getCacheServer(const string& url)
{
  //XXX: modify this to send own IP address
  longstring urlStr = url;
  auto ip = client->getCacheServer(urlStr);
  return *ip;
}

string
Client::getCacheContents(const string& cacheHost,
                         const string& url)
{
  longstring urlStr = url;
  
  auto fd = tcp_connect(cacheHost.c_str(), UNIQUE_CACHESERVER_PORT);
  auto cclient = new srpc_client<cache_api_v1>{fd.release()};
  auto res = cclient->getCacheContents(urlStr);

  string ret;
  ret.reserve(res->size());
  //XXX: For now we return a string
  //     We may be able to use memcpy here
  for (int i = 0; i < res->size(); i++) {
    ret += (*res)[i];
  }
  cout << "Received data size: " << ret.size() << endl;

  //deleting the client will terminate the connection
  delete cclient;
  return ret;
}
