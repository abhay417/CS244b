#ifndef __LRU_CACHE_H__
#define __LRU_CACHE_H__

#include <cstring>
#include <unordered_map>
#include "include/server.hh"

class lru_cache {
public:
  lru_cache(int max = 10);
  ~lru_cache();
  void set_max_size(int size);
  cache_data get(std::string key);
  void put(std::string key, cache_data value);
  bool contains(std::string key);
private:
  struct node {
    node* prev;
    node* next;
    cache_data data;

    node(cache_data data, node* prev = NULL, node* next = NULL) {
      this->data = data;
      this->prev = prev;
      this->next = next;
    }
  };
  void evict();
  void insert_head(node* n);
  void remove(node* n);

  std::unordered_map<std::string, node*> cache;
  int max_size;
  node* head;
  node* tail;
};

#endif
