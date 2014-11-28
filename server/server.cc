#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <thread>
#include <xdrpp/srpc.h>
#include <xdrpp/rpcbind.hh>
#include <xdrpp/socket.h>
#include "include/rpcconfig.h"

#include "serverimpl.hh"
#include <pthread.h>
#include <vector>
#include <map>
#include "include/helpers.hh" 

using namespace std;
using namespace xdr;

int
createBindListenSocket(int port) 
{
  //First create a socket
  int hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (hSocket == -1) {
    return -1;
  }

  //Then bind the socket to the port
  sockaddr_in sockAddr;
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(port);
  sockAddr.sin_addr.s_addr = INADDR_ANY;
  
  int res = bind(hSocket, reinterpret_cast<const sockaddr*>(&sockAddr),
           sizeof(sockaddr_in));
  if (res == -1) {
    close(hSocket);
    return -1;
  }

  //start listening on the socket
  res = listen(hSocket, SOMAXCONN);
  if (res == -1) {
    close(hSocket);
    return -1;
  }
  
  return hSocket;
}

void*
proxyServerLoop(void* dummy)
{
  //First create a socket to listen to proxy requests
  int hSocket = createBindListenSocket(UNIQUE_PROXY_PORT);
  if (hSocket == -1) {
    cerr << "Failed to create socket to listen to the proxy port" << endl;
  }
  
  //XXX: These variable will probably be required to process
  //     non GET/HEAD requests
  //char buffer[4096];
  //vector<uint8_t> request;
  while (true) {
    sockaddr_in clientSockAddr;
    unsigned clientSockSize = sizeof(clientSockAddr);
    int clientSocket = accept(hSocket,
                              reinterpret_cast<sockaddr *>(&clientSockAddr),
                              &clientSockSize);
    if (clientSocket == -1) {
      cerr << "Accept failed." << endl;
      continue;
    }
    
    //XXX: For now we only handle get and head requests
    //request.clear();
    //int totalBytesReceived = 0;
    
    //Get the headers first
    string header;
    if (!getHttpHeader(clientSocket, header)) {
      cerr << "Failed to get HTTP header" << endl;
      close(clientSocket);
      continue;
    }
    
    //Check if GET or HEAD
    bool isGetRequest;
    size_t firstSpaceInd = header.find(' ');
    string rType = header.substr(0, firstSpaceInd);
    if (rType != "HEAD" && rType != "GET") {
      cerr << "Not get or head request: " << rType << endl;
      close(clientSocket);
      continue;
    }
    if (rType == "HEAD") {
      isGetRequest = false;
    } else {
      isGetRequest = true;
    }
    
    //Parse the full request URL from the header
    size_t secondSpaceInd = header.find(' ', firstSpaceInd + 1);
    string requestUrl = header.substr(firstSpaceInd + 1,
                      secondSpaceInd - firstSpaceInd - 1);
    
    //Parse the Host: from the header
    string hostKey("Host: ");
    size_t hostInd = header.find(hostKey);
    size_t nextCRLF = header.find("\r\n", hostInd);
    string host = header.substr(hostInd + hostKey.length(),
                  nextCRLF - hostInd - hostKey.length());
    
    //XXX: Get the cache server to contact
    //     For now we need a cacheserver running on myth7
    string cacheServer("myth7");
    
    //Create a cache server client and get the content
    auto fd = tcp_connect(cacheServer.c_str(), UNIQUE_CACHESERVER_PORT);
    auto csClient = new srpc_client<cache_api_v1>{fd.release()};
    
    //Send the request
    cacheRequest cr;
    cr.host = host;
    cr.getRequest = isGetRequest;
    cr.requestUrl = requestUrl;
    cr.request = header;
    auto res = csClient->getCacheContents2(cr);
    cout << "Received from cache server: " << res->size() << endl; 
    //vector<uint8_t> response;
    //response.reserve(res->size());
    //for (int i = 0; i < res->size(); i++) {
    //  response[i] = (*res)[i];
    //}
    
    //Now we send back the content to the client
    int totalBytesSent = 0;
    do {
      int n = send(clientSocket, &((*res)[0]) + totalBytesSent,
                   res->size() - totalBytesSent, 0);
      if (n <= 0) {
        cerr << "Error writing to socket" << endl;
        close(clientSocket);
        continue;
      }
      totalBytesSent += n;
    } while (totalBytesSent < res->size());
    
    //Response sent back to client
    close(clientSocket);
  }
}

int main(int argc, const char *argv[])
{
    api_v1_server s;
    rpc_tcp_listener rl(tcp_listen(UNIQUE_MASTER_PORT, AF_INET));

    //XXX: We still need to tie together the Proxy server with the
    //     Master server so the Proxy can query the master to get
    //     the cache server to be used to handle requests
    //     Idea 1: Make an RPC call
    //     Idea 2: Plug in the Proxy code into the Master class
    //             so it can simply use the "_ring" map
    //Create a thread for the proxy server
    pthread_t proxyThread;
    int proxy_res = pthread_create(&proxyThread, NULL,
                                   proxyServerLoop, (void*)NULL);
    
    try {
        rl.register_service(s);
        rl.run();
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 0;
}

