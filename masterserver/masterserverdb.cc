
#include <assert.h>
#include <stdint.h>

#include <unistd.h>

#include <set>
#include <string>
#include <iostream>
#include <memory>
#include "masterserverdb.h"

using namespace std;

MasterServerDB::MasterServerDB()
{
}

MasterServerDB::~MasterServerDB()
{
}

bool
MasterServerDB::find(const string &key)
{
    return true;
}

string
MasterServerDB::get(const string &key)
{
    return "Hello World!";
}

bool
MasterServerDB::put(const string &path, const string &val)
{
    return true;
}

set<string>
MasterServerDB::list(const string &path)
{
   std::set<string> r;
   return r;
}
#if 0

int
main(int argc, const char *argv[])
{
  return 0;
}

#endif

