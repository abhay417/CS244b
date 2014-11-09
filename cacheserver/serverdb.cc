
#include <assert.h>
#include <stdint.h>

#include <unistd.h>

#include <set>
#include <string>
#include <iostream>
#include <memory>
#include "serverdb.h"

using namespace std;

ServerDB::ServerDB()
{
}

ServerDB::~ServerDB()
{
}

bool
ServerDB::find(const string &key)
{
    return true;
}

string
ServerDB::get(const string &key)
{
    return "Hello World!";
}

bool
ServerDB::put(const string &path, const string &val)
{
    return true;
}

set<string>
ServerDB::list(const string &path)
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

