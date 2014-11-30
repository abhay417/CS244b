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
#include "include/httpclient.hh"

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
proxyServerLoop(void* MasterServer)
{
  //First create a socket to listen to proxy requests
  int hSocket = createBindListenSocket(UNIQUE_PROXY_PORT);
  if (hSocket == -1) {
    cerr << "Failed to create socket to listen to the proxy port" << endl;
    return NULL;
  }
  
  //Setup master so we can access the _ring
  const api_v1_server* pMaster = (api_v1_server*) MasterServer;
  
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
    
    //Get the headers first
    string header;
    if (!getHttpHeader(clientSocket, header)) {
      close(clientSocket);
      continue;
    }
    
    //Check if GET, HEAD, or POST
    bool isHeadRequest;
    size_t firstSpaceInd = header.find(' ');
    string rType = header.substr(0, firstSpaceInd);
    if (rType != "HEAD" && rType != "GET" && rType != "POST") {
      cerr << "Not head, get or post request: " << rType << endl;
      close(clientSocket);
      continue;
    }
    if (rType == "HEAD") {
      isHeadRequest = true;
    } else {
      isHeadRequest = false;
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

    vector<uint8_t> response;
    if (rType == "POST") {
      //Copy the header to the request
      vector<uint8_t> request;
      request.reserve(header.size());
      request.insert(request.end(), (uint8_t *)header.c_str(),
                     ((uint8_t*)header.c_str()) + header.size());
      
      if (!getHTTPContent(clientSocket, header, request)) {
        //Failed to get the HTTP content
        close(clientSocket);
        continue;
      }
      
      httpclient originClient(host);
      int headerSize;
      response = originClient.sendPost(request);
    } else {
      //Get the cache server to contact
      uint128_t digest;
      getMD5Digest(requestUrl, &digest);
      string cacheServer = pMaster->getCacheServer(digest);
      
      //Create a cache server client and get the content
      //XXX:Do we need to delete the csClient?
      auto fd = tcp_connect(cacheServer.c_str(), UNIQUE_CACHESERVER_PORT);
      auto csClient = new srpc_client<cache_api_v1>{fd.release()};
      
      //Send the request
      cacheRequest cr;
      cr.host = host;
      cr.isHeadRequest = isHeadRequest;
      cr.requestUrl = requestUrl;
      cr.request = header;
      response = *(csClient->getCacheContents2(cr));
      
      cout << "Received from cache server: " << std::dec << response.size() << endl;
    }
    
    //Now we send the back the content to the client
    int totalBytesSent = 0;
    do {
      int n = send(clientSocket, &(response[0]) + totalBytesSent,
                   response.size() - totalBytesSent, 0);
      if (n <= 0) {
        cerr << "Error writing to socket" << endl;
        close(clientSocket);
        continue;
      }
      totalBytesSent += n;
    } while (totalBytesSent < response.size());
    
    //Response sent back to client
    close(clientSocket);
  }
  
  close(hSocket);
}

int main(int argc, const char *argv[])
{
    //Master server
    api_v1_server s;
    rpc_tcp_listener rl(tcp_listen(UNIQUE_MASTER_PORT, AF_INET));

    //ProxyServer
    pthread_t proxyThread;
    int proxy_res = pthread_create(&proxyThread, NULL,
                                   proxyServerLoop, (void*)&s);
    
    try {
        rl.register_service(s);
        rl.run();
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 0;
}

