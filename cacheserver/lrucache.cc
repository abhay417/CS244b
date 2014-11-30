#include <iostream>
#include <cstring>
#include <map>
#include <stdint.h>
#include <vector>
#include "cacheserver/lrucache.hh"

lru_cache::lru_cache(int max_size) {
  this->max_size = max_size;
  size_bytes = 0;
  head = NULL;
  tail = NULL;
}

lru_cache::~lru_cache() {
  while (head != NULL) {
    node* next = head->next;
    delete head;
    head = next;
  }
}

int lru_cache::size() {
  return size_bytes;
}

void lru_cache::set_max_size(int size) {
  max_size = size;
}

std::vector<uint8_t> & lru_cache::get(uint128_t digest) {
  node* n = cache.at(digest);
  remove(n);
  insert_head(n);
  return n->data;
}

void lru_cache::put(uint128_t digest, std::vector<uint8_t> value) {
  size_bytes += value.size();
  if (size_bytes >= max_size) {
    evict();
  }
  node* n;
  if (contains(digest)) {
    n = cache[digest];
    remove(n);
  } else {
    n = new node(digest, value);
    cache[digest] = n;
  }
  insert_head(n);
}

void lru_cache::erase(uint128_t digest) {
  node* removed = cache[digest];
  size_bytes -= removed->data.size();
  cache.erase(digest);
  remove(removed);
  delete removed;
}

bool lru_cache::contains(uint128_t digest) {
  return cache.find(digest) != cache.end();
}

std::ostream& operator<<(std::ostream& out, const lru_cache& cache) {
  out << "FORWARDS: {";
  lru_cache::node* n = cache.head;
  if (n != NULL) {
    out << n->data.size();
    n = n->next;
    while (n != NULL) {
      out << ", " << n->data.size();
      n = n->next;
    }
  }
  out << "} ";
  out << "BACKWARDS: {";
  n = cache.tail;
  if (n != NULL) {
    out << n->data.size();
    n = n->prev;
    while (n != NULL) {
      out << ", " << n->data.size();
      n = n->prev;
    }
  }
  out << "}";
  return out;
}

//typedef lru_cache_iter iterator;

lru_cache_iter lru_cache::begin() {
  return lru_cache_iter(*this, 0);
}

lru_cache_iter lru_cache::end() {
  return lru_cache_iter(*this, size());
}

void lru_cache::evict() {
  if (tail) {
    erase(tail->digest);
  }
}

void lru_cache::insert_head(lru_cache::node* n) {
  n->next = head;
  head = n;
  n->prev= NULL;
  if (!tail) {
    tail = n;
  } else {
    n->next->prev = n;
  }
}

void lru_cache::remove(lru_cache::node* n) {
  if (n->prev) {
    n->prev->next = n->next;
  } else {
    head = n->next;
  }
  if (n->next) {
    n->next->prev = n->prev;
  } else {
    tail = n->prev;
  }
}

lru_cache_iter::lru_cache_iter(lru_cache& cache, int size) {
  if (size == cache.size()) {
    node = NULL;
  } else {
    node = cache.head;
  }
}

uint128_t lru_cache_iter::operator*() {
  return node->digest;
}

lru_cache::node* lru_cache_iter::operator->() {
  return node;
}

lru_cache_iter & lru_cache_iter::operator++() {
  node = node->next;
  return *this;
}

lru_cache_iter lru_cache_iter::operator++(int) {
  lru_cache_iter clone = *this;
  ++(*this);
  return clone;
}

bool lru_cache_iter::operator==(const lru_cache_iter& other) {
  return node == other.node;
}

bool lru_cache_iter::operator!=(const lru_cache_iter& other) {
  return node != other.node;
}

