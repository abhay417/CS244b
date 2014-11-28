#include "helpers.hh"
#include "httpclient.hh"


using namespace std;

void
httpclient::open_socketfd(const string& hostname,
                          const string& port,
                          int flags,
                          int (*func)(int, const struct sockaddr*, socklen_t))
{
  //Create hints and get IP of the host
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  /* ai_flags indicating what type of connection there is
     flags is expected to be either PASSIVE or V4_MAPPED */
  hints.ai_flags = AI_ADDRCONFIG | flags;
  hints.ai_family = AF_INET;       //IPv4 address
  hints.ai_socktype = SOCK_STREAM; //Stream connection
  hints.ai_protocol = IPPROTO_TCP; //TCP connection

  //Get the host's IP
  struct addrinfo * hostaddresses = NULL;
  if (getaddrinfo(hostname.c_str(),
                  port.c_str(),
                  &hints,
                  &hostaddresses) != 0) {
    return;
  }

  /* creates a socket to that address */
  _socket = socket(hostaddresses->ai_family,
                   hostaddresses->ai_socktype,
                   hostaddresses->ai_protocol);
  if (_socket <= 0) {
    freeaddrinfo(hostaddresses);
    return;
  }

  /* either binds or connects to the address */
  if(func(_socket,
          hostaddresses->ai_addr,
          hostaddresses->ai_addrlen) == -1) {
    freeaddrinfo(hostaddresses);
    return;
  }

  freeaddrinfo(hostaddresses);
  _initialized = true;
}

httpclient::httpclient(string host, string port): _socket(0), _initialized(false)
{
  //Remove http:// if passed
  string httpPrefix("http://");
  if (strcasecmp(host.substr(0, httpPrefix.length()).c_str(),
                 httpPrefix.c_str()) == 0) {
    //Prefix matched
    host = host.substr(httpPrefix.length());
  }
  _host = host;

  //Port
  //XXX: Read the port from the host string
  // string port("80");

  //Connect to the server 
  open_socketfd(host, port, AI_V4MAPPED, &connect);
  if (!_initialized) {
    cerr << "Error connecting to server " << host << endl;
    return; 
  }
  //cout << "Successfully initialized connection" << endl; 
}

httpclient::~httpclient()
{
  if (_initialized && _socket) {
    close(_socket);
  }
}

//XXX: Consolidate common code from the following
//     two functions
vector<uint8_t>
httpclient::sendRequest(string queryStr,
                        int &headerSize,
                        bool getRequest)
{
  vector<uint8_t> response;
  headerSize = 0;
  //string req = "HEAD " + queryStr + " HTTP/1.1\r\nHost: " +
  string req = getRequest ? "GET " : "HEAD ";
  req += queryStr + " HTTP/1.1\r\nHost: " +
                     _host + "\r\nAccept: */*\r\n\r\n";
  
  //Send the request
  int n = send(_socket, req.c_str(), req.size(), 0);
  if (n <= 0) {
    cerr << "Error writing to socket" << endl;
    return response;
  }
 
  //Read the header first that is until we encounter a \r\n\r\n
  string responseHeader;
  if (!getHttpHeader(_socket, responseHeader)) {
    cerr << "Failed to get HTTP header" << endl;
    return response;
  }
  headerSize = responseHeader.size();
  
  //Get the content length
  string cLenStr("Content-Length:");
  size_t cLenIndEnd = responseHeader.find(cLenStr) + cLenStr.length();
  size_t nextCRLF = responseHeader.find("\r\n", cLenIndEnd);
  string contentLenStr = responseHeader.substr(cLenIndEnd,
                                               nextCRLF - cLenIndEnd);
  int contentLen = atoi(contentLenStr.c_str());

  //Copy the header to response
  response.reserve(responseHeader.size());
  response.insert(response.end(), (uint8_t *)responseHeader.c_str(),
                  (uint8_t *) (responseHeader.c_str()) + responseHeader.size());

  if (!getRequest) {
    //Return the response header for head request
    cout << "HEAD response complete" << endl;
    return response;
  }

  //If GET request the loop to get the remaining bytes
  uint8_t buf[4096];
  int totalBytesReceived = 0;
  while (totalBytesReceived < contentLen) {
    n = recv(_socket, buf, sizeof(buf), 0);
    if (n <= 0) {
      cerr << "Failed to recv data" << endl;
    }
    totalBytesReceived += n;
    response.reserve(totalBytesReceived);
    response.insert(response.end(), buf, buf + n); 
  }
  
  cout << "GET response complete" << endl;
  return response;
}

vector<uint8_t>
httpclient::sendRequest2(string request,
                         int &headerSize,
                         bool getRequest)
{
  vector<uint8_t> response;
  headerSize = 0;
  
  //Send the request
  int n = 0;
  int totalBytesSent = 0;
  do {
    n = send(_socket, request.c_str() + totalBytesSent,
             request.size() - totalBytesSent, 0);
    if (n <= 0) {
      cerr << "Error writing to socket" << endl;
      return response;
    }
    totalBytesSent += n;
  } while (totalBytesSent < request.size());
 
  //Read the header first that is until we encounter a \r\n\r\n
  string responseHeader;
  if (!getHttpHeader(_socket, responseHeader)) {
    cerr << "Failed to get HTTP header" << endl;
    return response;
  }
  headerSize = responseHeader.size();
  
  //Get the content length
  string cLenStr("Content-Length:");
  size_t cLenIndEnd = responseHeader.find(cLenStr) + cLenStr.length();
  size_t nextCRLF = responseHeader.find("\r\n", cLenIndEnd);
  string contentLenStr = responseHeader.substr(cLenIndEnd,
                                            nextCRLF - cLenIndEnd);
  int contentLen = atoi(contentLenStr.c_str());

  //Copy the header to resPonse
  response.reserve(responseHeader.size());
  response.insert(response.end(), (uint8_t *)responseHeader.c_str(),
                  (uint8_t *) (responseHeader.c_str()) + responseHeader.size());

  if (!getRequest) {
    //Return the response header for head request
    cout << "HEAD response complete" << endl;
    return response;
  }

  //If GET request the loop to get the remaining bytes
  uint8_t buf[4096];
  int totalBytesReceived = 0;
  while (totalBytesReceived < contentLen) {
    n = recv(_socket, buf, sizeof(buf), 0);
    if (n <= 0) {
      cerr << "Failed to recv data" << endl;
    }
    totalBytesReceived += n;
    response.reserve(totalBytesReceived);
    response.insert(response.end(), buf, buf + n); 
  }
  
  cout << "GET response complete" << endl;
  return response;
}


