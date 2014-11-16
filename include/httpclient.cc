#include "httpclient.hh"

using namespace std;

int
httpclient::open_socketfd(const string& hostname,
                          const string& port,
                          int flags,
                          int (*func)(int, const struct sockaddr*, socklen_t))
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
  if(getaddrinfo(hostname.c_str(), port.c_str(), &hints, &hostaddresses) != 0)
		return -3;
	/* creates a socket to that address */
	if((socketfd = socket(hostaddresses->ai_family, hostaddresses->ai_socktype,
                        hostaddresses->ai_protocol)) == -1)
	  return -2;
  /* either binds or connects to the address */
	if(func(socketfd, hostaddresses->ai_addr, hostaddresses->ai_addrlen) == -1)
		return -1;
	freeaddrinfo(hostaddresses);
	return socketfd;
}

vector<uint8_t>
httpclient::sendRequest(string host, string queryStr)
{
  string req = "GET " + queryStr + " HTTP/1.1\r\nHost: " +
                     host + ":80\r\nAccept: */*\r\n\r\n";
  string port = "80";
  int socketfd, fd;
  
  if ((socketfd = open_socketfd(host, port, AI_V4MAPPED, &connect)) < 0) {
    std::cerr << "Error connecting to server " << host << std::endl;
  }// else {
    // std::cout << "cacheclient connected to source host" << std::endl;
  //}
  
  uint8_t buf[4096];
  int n = write(socketfd, req.c_str(), req.size());
  if (n <= 0) {
    std::cerr << "Error writing to socket" << std::endl;
  } else {
    // std::cout << "Sucessfully sent request " << std::endl << req << std::endl;
    n = 0;
    n = read(socketfd, buf, sizeof(buf));
    if (n <= 0) {
      std::cerr << "read response failed, n = " << n << std::endl;
    }
    
    //XXX: Currently we are only handling 4096 byte responses.
    //     We need to call 'read' in a loop to handle larger responses
    
    // std::cout << "response: " << buf << " : bytes: " << n << std::endl;
  }
  close(socketfd);            
  
  if (n > 0) { 
    return vector<uint8_t>(buf, buf + n);
  } else {
    return vector<uint8_t>();
  }
}
