// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "masterserver/masterserverimpl.hh"

std::unique_ptr<Result>
api_v1_masterserver::list(std::unique_ptr<longstring> arg)
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
api_v1_masterserver::put(std::unique_ptr<kvpair> arg)
{
  std::cout << "put called" << std::endl;
  std::string key = arg->val;
  Result * result = new Result();
  result->success = true;
  result->val = 0;
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_masterserver::remove(std::unique_ptr<longstring> arg)
{
  std::cout << "remove called" << std::endl;
  std::string key = arg->data();
  Result * result = new Result();
  result->success = true;
  result->val = 0;
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_masterserver::get(std::unique_ptr<longstring> arg)
{
  std::cout << "get called" << std::endl;
  std::string key = arg->data();
  Result * result = new Result();
  result->success = true;
  result->val = 0;
  std::unique_ptr<Result> res(result);
  return res;
}


