// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "cacheserver/cacheserverimpl.hh"

std::unique_ptr<Result>
api_v1_cacheserver::list(std::unique_ptr<longstring> arg)
{
  std::cout << "list called" << std::endl;
  std::string key = arg->data();
  Result * result = new Result();
  result->success = true;
  result->val = 0;
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_cacheserver::get(std::unique_ptr<longstring> arg)
{
  std::cout << "get called" << std::endl;
  std::string key = arg->data();
  Result * result = new Result();
  result->success = true;
  result->val = 0;
  std::unique_ptr<Result> res(result);
  return res;
}


