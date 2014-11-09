// Scaffolding originally generated from include/server.x.
// Edit to add functionality.

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "server/serverimpl.hh"

bool 
api_v1_server::isPathGood(std::string path) {
  const int NUM_BAD_CHARS = 9;
  char badChars [NUM_BAD_CHARS] = {'\\', '?', '%', '*', '|', '"', '<', '>', ' '};
  for (int i = 0; i < NUM_BAD_CHARS; i++) {
    if (path.find(badChars[i]) != std::string::npos) return false;
  }
  if (path[0] != '/') return false;
  else if ((path.length() > 1) && (path[path.length() - 1] == '/')) return false;
  else if (path.find("//") != std::string::npos) return false;
  else return true;
}

//c++ string split fn from stack overflow
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s,delim, elems);
  return elems;
}

bool
api_v1_server::checkParents(std::string path) {
  std::vector<std::string> nodes = split(path, '/');
  std::string currentPath = "";
  for (int i = 1; i < nodes.size() - 1; i++) {
    std::string newPath = currentPath + "/" + nodes[i];
    currentPath = newPath;
    if (!db.hasKey(currentPath)) return false;
  }
  return true;
}

std::unique_ptr<Result>
api_v1_server::create(std::unique_ptr<kvpair> arg)
{
  std::string key = arg->key;
  std::string val = arg->val;
  Result * result = new Result();

  try {
    if (!isPathGood(key) || (key.compare("/") == 0)) {
      std::cout << "Create Failed: Path: " << key << " is bad" << std::endl;
      result->success = false;
      result->code = 3;
    } else if (db.hasKey(key)) {
      std::cout << "Created " << key << " Failed" << std::endl;
      result->success = false;
      result->code = 5;
    } else if (!checkParents(key)) {
      std::cout << "Create Failed: one of the parents in " << key << " does not exist" << std::endl;
      result->success = false;
      result->code = 1;
    } else {
      db.set(key, val);
      result->success = true;
      std::cout << "Created " << key << " Succeded" << std::endl;
    }
  } catch (...) {
    result->success = false;
    result->code = 4;
  }
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_server::remove(std::unique_ptr<longstring> arg)
{
  std::string key = arg->data();
  Result * result = new Result();

  try {
    if (!isPathGood(key) || (key.compare("/") == 0)) {
      result->success = false;
      result->code = 3;
      std::cout << "Path: " << key << " is bad" << std::endl;
    } else if (!db.hasKey(key)) {
      std::cout << "Remove failed, key " << key << " does not exist" << std::endl;
      result->code = 0;
      result->success = false;
    } else if (db.list(key).size() > 0) {
      std::cout << "Remove failed, key " << key << " has children nodes" << std::endl;
      result->code = 2;
      result->success = false;
    } else {
      db.remove(key);
      result->success = true;
    }
  } catch (...) {
    result->success = false;
    result->code = 4;
  }
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_server::set(std::unique_ptr<kvpair> arg)
{
  std::string key = arg->key;
  Result * result = new Result();

  try {
    if (!isPathGood(key) || (key.compare("/") == 0)) {
      result->success = false;
      result->code = 3;
      std::cout << "Set failed, path: " << key << " is bad" << std::endl;
    } else if (!checkParents(key)) {
      std::cout << "Set failed, key " << key << " has parents that do not exist" << std::endl;
      result->code = 2;
      result->success = false;
    } else if (!db.hasKey(key)) {
      std::cout << "Set failed, key " << key << "does not exist" << std::endl;
      result->code = 0;
      result->success = false;
    } else {
      db.set(key, arg->val);
      result->success = true;
    }
  } catch (...) {
    result->success = false;
    result->code = 4;
  }  
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_server::list(std::unique_ptr<longstring> arg)
{
  std::string key = arg->data();
  Result * result = new Result();
  try {
    if (!isPathGood(key)) {
      result->success = false;
      result->code = 3;
      std::cout << "List failed, path: " << key << " is bad" << std::endl;
    } else if (!checkParents(key)) {
      std::cout << "List failed, key " << key << " has parents that do not exist" << std::endl;
      result->code = 2;
      result->success = false;
    } else if ((!db.hasKey(key)) && (key.compare("/") != 0)) {
      std::cout << "List failed, key " << key << "does not exist" << std::endl;
      result->code = 0;
      result->success = false;
    } else {
      std::set<std::string> nodeSet = db.list(key);
      std::string csv = "";
      for (std::set<std::string>::iterator it = nodeSet.begin(); it != nodeSet.end() ; it++) {
        std::string newCsv;
        if (it != nodeSet.begin()) { 
	  std::string node = (*it).substr((*it).find_last_of('/') + 1);
          newCsv = csv + "," + node;
        } else {
	  std::string node = (*it).substr((*it).find_last_of('/') + 1);
          newCsv = node;
	}
	csv = newCsv;
      }
      result->success = true;
      result->val = csv;
    }
  } catch (...) {
    result->success = false;
    result->code = 4;
  }
  std::unique_ptr<Result> res(result);
  return res;
}

std::unique_ptr<Result>
api_v1_server::get(std::unique_ptr<longstring> arg)
{
  std::string key = arg->data();
  Result * result = new Result();
  try {
    if (!isPathGood(key)) {
      result->success = false;
      result->code = 3;
      std::cout << "Get failed, path: " << key << " is bad" << std::endl;
    } else if (!checkParents(key)) {
      std::cout << "Get failed, key " << key << " has parents that do not exist" << std::endl;
      result->code = 2;
      result->success = false;
    } else if (!db.hasKey(key)) {
      std::cout << "Get failed, key " << key << "does not exist" << std::endl;
      result->code = 0;
      result->success = false;
    } else {
      result->success = true;
      result->val = db.get(key);
    }
  } catch (...) {
    result->success = false;
    result->code = 4; 
  }
  std::unique_ptr<Result> res(result);
  return res;
}


