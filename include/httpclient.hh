// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

class httpclient {

	int open_socketfd(char *hostname, char* port, int flags, int (*func)(int, const struct sockaddr*, socklen_t))
	{
		int socketfd;
		struct addrinfo hints, *hostaddresses = NULL;
		memset(&hints, 0, sizeof(hints));
		/* ai_flags indicating what type of connection there is
		   flags is expected to be either PASSIVE or V4_MAPPED */
		hints.ai_flags = AI_ADDRCONFIG | flags;
		/* indicates an ip4 address */
		hints.ai_family = AF_INET;
		/* looking for a stream connection */
		hints.ai_socktype = SOCK_STREAM;
		/* asks the DNS server for the address */
		if(getaddrinfo(hostname, port, &hints, &hostaddresses) != 0)
			return -3;
		/* creates a socket to that address */
		if((socketfd = socket(hostaddresses->ai_family, hostaddresses->ai_socktype, hostaddresses->ai_protocol)) == -1)
			return -2;
		/* either binds or connects to the address */
		if(func(socketfd, hostaddresses->ai_addr, hostaddresses->ai_addrlen) == -1)
			return -1;
		freeaddrinfo(hostaddresses);
		return socketfd;
	}

	public:
	std::string sendRequest(std::string host, std::string queryStr) {
	  std::string req = "GET " + queryStr + " HTTP/1.1\r\nHost: " + host + ":80\r\nAccept: */*\r\n\r\n";
	  char * creq = strdup(req.c_str());
	  int socketfd, fd;
	  char * chost = strdup(host.c_str());
	  char * port = "80\0";
	  
	  if ((socketfd = open_socketfd(chost, "80", AI_V4MAPPED, &connect)) < 0) {
	    std::cerr << "Error connecting to server " << host << std::endl;
	  } else {
	    // std::cout << "cacheclient connected to source host" << std::endl;
	  }
	  
	  char buf[4096];
	  int n = write(socketfd, creq, sizeof(creq) * strlen(creq));
	  if (n <= 0) {
	    std::cerr << "Error writing to socket" << std::endl;
	  } else {
	    // std::cout << "Sucessfully sent request " << std::endl << req << std::endl;
	    n = read(socketfd, buf, sizeof(buf));
	    if (n <= 0) {
	      std::cerr << "read response failed, n = " << n << std::endl;
	    }
	    // std::cout << "response: " << buf << " : bytes: " << n << std::endl;
	    close(socketfd);
	  }
	  return std::string(buf);
	}

};
