
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
#include <include/cacheserver.hh>
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
    std::set<std::string> retSet;
    return retSet;
}

