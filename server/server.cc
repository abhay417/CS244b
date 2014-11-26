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
CreateBindListenSocket(int port) 
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

int
ProxyServerLoop(void *)
{
	//First create a socket to listen to proxy requests
	int hSocket = CreateBindListenSocket(UNIQUE_PROXY_PORT);
	if (hSocket == -1) {
		cerr << "Failed to create socket to listen to the proxy port" << endl;
	}
	
	char buffer[4096];
	vector<uint8_t> request;
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
		request.clear();
		int totalBytesReceived = 0;
		
		//Get the headers first
		string header;
		if (!getHttpHeader(clientSocket, header)) {
			cerr << "Failed to get HTTP header" << endl;
			close(clientSocket);
			continue;
		}
		
		//Check if GET or HEAD
		size_t firstSpaceInd = header.find(' ');
		string rType = header.substr(0, firstSpaceInd);
		if (rType != "HEAD" && rType != "GET") {
			cerr << "Not get or head request: " << rType << endl;
			close(clientSocket);
			continue;
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
		
		//Get the cachesever to contact for requestURL
		//Create a connection to the cacheserver and
		//Send the host, requestURL and the header
		//Then reply to client with the data returned by the cacheserver
		
		close(clientSocket);
	}
}


int main(int argc, const char *argv[])
{
    api_v1_server s;
    rpc_tcp_listener rl(tcp_listen(UNIQUE_MASTER_PORT, AF_INET));

    try {
        rl.register_service(s);
        rl.run();
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 0;
}

