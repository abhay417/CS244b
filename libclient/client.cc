
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>


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

    auto fd = tcp_connect(host.c_str(), UNIQUE_RPC_PORT);
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

bool
Client::create(const std::string &path, const std::string &val)
{
    kvpair args;

    args.key = path;
    args.val = val;

    auto r = client->create(args);
    bool ret = r->success;
    if (!ret) {
      if (r->code < 5) {
        throw ClientException((ClientError)r->code);
      }
    }
    return ret;
}

bool
Client::remove(const std::string &path)
{
    auto r = client->remove(path);
    bool ret = r->success;
    if (!ret) {
      if (r->code < 5)
        throw ClientException((ClientError)r->code);
    }
    return ret;
}

std::string
Client::get(const std::string &path)
{
    auto r = client->get(path);
    bool ret = r->success;
    if (!ret) {
      if (r->code < 5)
        throw ClientException((ClientError)r->code);
    }
    return r->val;
}

void
Client::set(const std::string &path, const std::string &val)
{
    kvpair args;

    args.key = path;
    args.val = val;

    auto r = client->set(args);
    bool ret = r->success;
    if (!ret) {
      if (r->code < 5) {
        throw ClientException((ClientError)r->code);
      }
    }
}

//c++ string split fn from stack overflow
std::set<std::string> &split(const std::string &s, char delim, std::set<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.insert(item);
  }
  return elems;
}

std::set<std::string> split(const std::string &s, char delim) {
  std::set<std::string> elems;
  split(s,delim, elems);
  return elems;
}

std::set<string>
Client::list(const string &path)
{
    auto r = client->list(path);
    bool ret = r->success;
    if (!ret) {
      if (r->code < 5)
        throw ClientException((ClientError)r->code);
    }
    std::string val = r->val;
    std::set<std::string> retSet = split(val, ',');
    return retSet;
}

