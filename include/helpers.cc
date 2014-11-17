#include "helpers.hh"

uint128_t
combineLowHigh(uint64_t low,
               uint64_t high)
{
  uint128_t res = high << 64;
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

string
getOwnAddress()
{
  //XXX: Add code to return own IP address as a string
  char buffer[256];
  gethostname(buffer, 256);
  return string(buffer);
}
