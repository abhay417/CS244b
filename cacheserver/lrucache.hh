#ifndef __LRU_CACHE_H__
#define __LRU_CACHE_H__

#include <iostream>
#include <cstring>
#include <map>
#include <stdint.h>
#include <vector>
#include "include/server.hh"
#include "include/helpers.hh"

#define HUNDRED_MB_IN_BYTES 100000000
#define TWOFIVE_PERCENT 70000000 / 4
#define ONETWOFIVE_PERCENT 70000000 / 8
#define SIXSEVENFIVE_PERCENT 70000000 / 16

class lru_cache_iter;

class lru_cache {
public:
  friend class lru_cache_iter;

  lru_cache(int max_size = HUNDRED_MB_IN_BYTES);
  ~lru_cache();

  int size();
  void set_max_size(int size);
  std::vector<uint8_t> & get(uint128_t digest);
  void put(uint128_t digest, std::vector<uint8_t> value);
  void erase(uint128_t digest);
  bool contains(uint128_t digest);
  friend std::ostream& operator<<(std::ostream& out, const lru_cache& cache);

  //typedef lru_cache_iter iterator;
  lru_cache_iter begin();
  lru_cache_iter end();

private:
  struct node {
    node* prev;
    node* next;
    uint128_t digest;
    std::vector<uint8_t> data;

    node(uint128_t digest, std::vector<uint8_t> data, node* prev = NULL, node* next = NULL) {
      this->digest = digest;
      this->data = data;
      this->prev = prev;
      this->next = next;
    }
  };

  void evict();
  void insert_head(node* n);
  void remove(node* n);

  std::map<uint128_t, node*> cache;
  int max_size;
  int size_bytes;
  node* head;
  node* tail;
};

class lru_cache_iter {
public:
  typedef lru_cache_iter iterator;

  lru_cache_iter(lru_cache& cache, int size);
  uint128_t operator*();
  lru_cache::node* operator->();
  iterator & operator++();
  iterator operator++(int);

  bool operator==(const lru_cache_iter& other);
  bool operator!=(const lru_cache_iter& other);

private:
  lru_cache::node* node;
};

#endif
