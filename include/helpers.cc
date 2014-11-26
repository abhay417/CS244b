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
getHttpHeader(int socket,
              string& header)
{
	//Get the first 4 bytes
	char buf[4];
	int n = recv(socket, buf, 4, 0);
	if (n != 4) {
		//XXX: Is it better to fail or to loop here?
		cerr << "Failed to recv data." << endl;
		return false;
	}
	header.insert(0, buf, 4);
	
	//Loop until we get the CRLFCRLF that signals the end
	//of the header
	while (header.substr(header.length() - 4, 4) != "\r\n\r\n") {
		n = recv (socket, buf, 1, 0);
		if (n != 1) {
		  cerr << "Failed to recv data" << endl;
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
