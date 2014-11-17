#ifndef __SERVER_HELPERS_HH_INCLUDED__
#define __SERVER_HELPERS_HH_INCLUDED__ 1

#include <unistd.h>
#include <time.h>
#include <openssl/md5.h>
#include <string>

using namespace std;

typedef unsigned int uint128_t __attribute__((mode(TI)));

uint128_t combineLowHigh(uint64_t low, uint64_t high);
uint128_t getMonotonicNsec();
void getMD5Digest(const string& str, uint128_t* digest);
string getOwnAddress();

#endif
