#include "helpers.hh"

uint128_t
combineLowHigh(uint64_t low,
               uint64_t high)
{
  uint128_t res = high;
  res = res << 64;
  res +=  low;
  return res;
}

void
getMD5Digest(const string& str, uint128_t* digest) 
{
  MD5(reinterpret_cast<const unsigned char *>(str.c_str()),
      str.length(),
      reinterpret_cast<unsigned char *>(digest));
}

uint128_t
getMonotonicNsec() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  uint128_t nsec = ts.tv_sec * 1000000000 + ts.tv_nsec;
  return nsec;
}

bool
getHTTPContent(int socket,
               const string& header,
               vector<uint8_t>& content)
{
  //First get the content length from the header
  string cLenStr("Content-Length:");
  size_t cLenIndEnd = header.find(cLenStr);
  if (cLenIndEnd == string::npos) {
    //Content length not found
    //Assuming length of 0
    return true;
  }
  cLenIndEnd += cLenStr.length();
  size_t nextCRLF = header.find("\r\n", cLenIndEnd);
  string contentLenAsStr = header.substr(cLenIndEnd,
                                         nextCRLF - cLenIndEnd);
  int contentLen = atoi(contentLenAsStr.c_str());
  // IF content length is actually zero, nothing left to read
  if (contentLen == 0) {
    return true;
  }

  //Then get the content
  uint8_t buf[4096];
  int initSize = content.size();
  int totalBytesReceived = 0;
  do {
    int n = recv(socket, buf, sizeof(buf), 0);
    if (n <= 0) {
      cerr << "(getHTTPContent)Failed to recv data. N="
           << n << endl;
      return false;
    }
    totalBytesReceived += n;
    content.reserve(totalBytesReceived + initSize);
    content.insert(content.end(), buf, buf + n);
  } while (totalBytesReceived < contentLen);
  
  return true;
}

bool
getHttpHeader(int socket,
              string& header)
{
  header.clear();
  header.reserve(4096);
  //Get the first 4 bytes
  char buf[4];
  int n = recv(socket, buf, 4, 0);
  if (n != 4) {
    //XXX: Is it better to fail or to loop here?
    cerr << "(getHTTPHeader)Failed to recv 4 bytes. N=" << n << endl;
    return false;
  }
  header.insert(0, buf, 4);
  
  //Loop until we get the CRLFCRLF that signals the end
  //of the header
  while (header.substr(header.length() - 4, 4) != "\r\n\r\n") {
    n = recv (socket, buf, 1, 0);
    if (n != 1) {
      cerr << "(getHTTPHeader)Failed to recv 1 byte. N="
           << n << endl;
      return false;
    }
    header += buf[0];
  }
  return true;
}

string
getOwnAddress()
{
  //XXX: Add code to return own IP address as a string
  char buffer[256];
  gethostname(buffer, 256);
  return string(buffer);
}

void
printUint128(uint128_t num)
{
  uint64_t low = (uint64_t) num;
  uint64_t high = (uint64_t) (num >> 64);
  cout << hex << high << " " << hex << low << endl;
}
