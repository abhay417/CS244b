#include "helpers.hh"
#include "rpcconfig.h"

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
getExactNumBytesFromSocket(int socket,
                   int totalNumBytesToGet,
                   vector<uint8_t>& content)
{
  uint8_t buf[8192];
  int intialSize = content.size();
  int totalBytesReceived = 0;
  do {
    int bytesLeft = totalNumBytesToGet - totalBytesReceived;
    int numBytesToRecv = (bytesLeft > sizeof(buf)) ? sizeof(buf) : bytesLeft;
    int n = recv(socket, buf, numBytesToRecv, 0);
    if (n <= 0) {
      cerr << "(getBytesFromSocket)Failed to recv data. N="
           << n << endl;
      return false;
    }
    totalBytesReceived += n;
    content.reserve(totalBytesReceived + intialSize);
    content.insert(content.end(), buf, buf + n);
  } while (totalBytesReceived < totalNumBytesToGet);
  
  return true;
}

string
getBytesUntilFirstCRLF(int socket)
{
  string oneLine;
  char buf;
  do {
    int n = recv(socket, &buf, 1, 0);
    if (n != 1) {
      cerr << "(getBytesUntilFirstCRLF) Failed to recv data" << endl;
      return string();
    }
    oneLine += buf; 
  } while (buf != '\n');

  return oneLine;
}

bool
getChunkedDataFromSocket(int socket,
                         vector<uint8_t>& content)
{
  int chunkSize;
  do {
    //Get content till the first CRLF
    string chunkSizeLine = getBytesUntilFirstCRLF(socket);
    if (chunkSizeLine.empty()) {
      return false;
    }
    
    //Get the value of the chunk size
    chunkSize = 0;
    {
      stringstream s;
      size_t crInd = chunkSizeLine.find('\r');
      s << std::hex << chunkSizeLine.substr(0, crInd);
      s >> chunkSize;
    }
    
    //Add the link to the content and get the chunk
    content.reserve(content.size() + chunkSizeLine.size());
    content.insert(content.end(), (uint8_t*) chunkSizeLine.c_str(),
                   ((uint8_t*) (chunkSizeLine.c_str())) + chunkSizeLine.size());
    
    //Get the chunk
    //+2 for the ending CRLF
    getExactNumBytesFromSocket(socket, chunkSize + 2, content);

  } while (chunkSize > 0);
  
  return true;
}

bool
getHTTPContent(int socket,
               const string& header,
               vector<uint8_t>& content)
{
  //First get the content length from the header
  size_t nextCRLF = -1;
  string cLenStr("Content-Length:");
  size_t cLenIndEnd = header.find(cLenStr);
  if (cLenIndEnd == string::npos) {
    cLenIndEnd = header.find("content-length:");
  }
  if (cLenIndEnd == string::npos) {
    //Content length not found
    //Check if the data is encoded as chunks
    string encodingStr("Transfer-Encoding:");
    size_t encodingInd = header.find(encodingStr);
    if (encodingInd == string::npos) {
      encodingInd = header.find("transfer-encoding:");
    }
    if (encodingInd == string::npos) {
      //Response is not encoded so assuming length of 0
      return true;
    }
    
    encodingInd += encodingStr.length();
    nextCRLF = header.find("\r\n", encodingInd);
    string encodeType = header.substr(encodingInd,
                                        nextCRLF - encodingInd);
    if (encodeType.find("chunked") == string::npos) {
      //Not chunked encoding
      return true;
    }
    
    if (!getChunkedDataFromSocket(socket, content)) {
      return false;
    }
    
    return true;
  }
  cLenIndEnd += cLenStr.length();
  nextCRLF = header.find("\r\n", cLenIndEnd);
  string contentLenAsStr = header.substr(cLenIndEnd,
                                         nextCRLF - cLenIndEnd);
  int contentLen = atoi(contentLenAsStr.c_str());
  // IF content length is actually zero, nothing left to read
  if (contentLen == 0) {
    return true;
  }

  //Then get the content
  if (!getExactNumBytesFromSocket(socket, contentLen, content)) {
    return false;
  }
  
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

  if (DEBUG_MODE) {
    cout << "(getHTTPHeader)Header: " << header << endl;
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
