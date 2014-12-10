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
#include <semaphore.h>
#include <vector>
#include <queue>
#include <map>
#include <sstream>
#include "include/helpers.hh" 
#include "include/httpclient.hh"

#define MAX_PACKET_SIZE 32000

using namespace std;
using namespace xdr;

//Proxy worker thread classes and defines
#define NUM_PROXY_WORKERS 4
struct requestTask {
  int socket;
  string header;
};
queue<requestTask> requestQueue;
pthread_mutex_t queueMutex;
sem_t queueSema;

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


void* proxyServerWorkerLoop(void *MasterServer)
{
  //Setup master so we can access the _ring
  const api_v1_server* pMaster = (api_v1_server*) MasterServer;
  
  
  bool nameRetrieved = false;
  string myName;
  while (true) {
    //Semaphore down
    sem_wait(&queueSema);
    
    //Get self name
    if (!nameRetrieved) {
      char buffer[256];
      pthread_t me = pthread_self();
      pthread_getname_np(me, buffer, sizeof(buffer));
      myName = buffer;
      nameRetrieved = true;
    }
    
    
    pthread_mutex_lock(&queueMutex);
    //XXX:Is there a better way to get the first element from the queue
    requestTask currTask = requestQueue.front();
    requestQueue.pop();
    pthread_mutex_unlock(&queueMutex);
    
    int& clientSocket = currTask.socket;
    string& header = currTask.header;
    
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
      
      cout << "(" << myName << ")Received content from origin server: " 
           << std::dec << response.size() << endl;
    } else {
      try {
      //Get the cache server to contact
      uint128_t digest;
      getMD5Digest(requestUrl, &digest);
      string cacheServer = pMaster->getCacheServer(digest);
      
      //Create a cache server client and get the content
      //XXX:Do we need to delete the csClient?
      auto fd = tcp_connect(cacheServer.c_str(), UNIQUE_CACHESERVER_PORT);
      auto csClient = new srpc_client<cache_api_v1>{fd.get()};
      
      //Send the request
      cacheRequest cr;
      cr.host = host;
      cr.isHeadRequest = isHeadRequest;
      cr.requestUrl = requestUrl;
      cr.request = header;
      response = *(csClient->getCacheContents2(cr));
      
      delete csClient;
      fd.clear();
      cout << "(" << myName << ")Received content from cache server: " 
           << std::dec << response.size() << endl;
      } catch (...) {
        std::cerr << "Error occured trying to get content from cache server" << std::endl;
      }
    }
    
    //Now we send the back the content to the client
    int responseSize = response.size();
    int bytesLeft = responseSize;
    do {
      int bytesToSend = (bytesLeft < MAX_PACKET_SIZE) ? bytesLeft : MAX_PACKET_SIZE;
      int n = send(clientSocket, &(response[0]) + responseSize - bytesLeft,
                   bytesToSend, 0);
      if (n <= 0) {
        cerr << "Error writing to socket" << endl;
        break;
      }
      bytesLeft -= n;
    } while (bytesLeft > 0);
    
    //Response sent back to client
    close(clientSocket);
  }
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
  
  //Initialize all the synchronization primitives
  pthread_mutex_init(&queueMutex, NULL);
  sem_init(&queueSema, 0, 0); //All proxy workers should block here
  
  //Start the worker threads
  pthread_t proxyWorkerThreads[NUM_PROXY_WORKERS];
  for (int i = 0; i < NUM_PROXY_WORKERS; i++) {
    int res = pthread_create(&(proxyWorkerThreads[i]), NULL,
                   proxyServerWorkerLoop, MasterServer);
    stringstream name;
    name << "ProxyWorker" << i;
    pthread_setname_np(proxyWorkerThreads[i], name.str().c_str());
    cout << "Created proxy worker " << name.str() << "(" << proxyWorkerThreads[i] 
         << "). Result: " << res << endl;
  }
  
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
    
    //Create a new task and add it to the queue
    requestTask rTask;
    rTask.socket = clientSocket;
    rTask.header = header;
    pthread_mutex_lock(&queueMutex);
    requestQueue.push(rTask);
    pthread_mutex_unlock(&queueMutex);
    
    //Signal the semaphore about the new task
    sem_post(&queueSema); 
  }
  
  //Cleanup
  sem_destroy(&queueSema);
  pthread_mutex_destroy(&queueMutex);
  close(hSocket);
}

int main(int argc, const char *argv[])
{
    //No SIGPIPE errs
    signal(SIGPIPE, SIG_IGN);

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

