#ifndef __SERVER_HTTPCLIENT_HH_INCLUDED__
#define __SERVER_HTTPCLIENT_HH_INCLUDED__ 1

#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

class httpclient {
private:
  bool _initialized;
  int _socket;
  string _host;
	void open_socketfd(const string& hostname,
                     const string& port,
                     int flags,
                     int (*func)(int, const struct sockaddr*, socklen_t));
public:
  httpclient(string host);
  ~httpclient();
	vector<uint8_t> sendRequest(string queryStr,
                              int &headerSize,
                              bool getRequest = true);
};

#endif
