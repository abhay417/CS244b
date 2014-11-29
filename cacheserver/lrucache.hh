#ifndef __LRU_CACHE_H__
#define __LRU_CACHE_H__

#include <iostream>
#include <cstring>
#include <map>
#include "include/server.hh"
#include "include/helpers.hh"

// All defined here to avoid template shenanigans

template <typename T>
class lru_cache_iter;

template <typename T>
class lru_cache {
public:
  friend class lru_cache_iter<T>;

  lru_cache(int max_size = 10) {
    this->max_size = max_size;
    head = NULL;
    tail = NULL;
  }

  ~lru_cache() {
    while (head != NULL) {
      node* next = head->next;
      delete head;
      head = next;
    }
  }

  int size() {
    return cache.size();
  }

  void set_max_size(int size) {
    max_size = size;
  }

  T & get(uint128_t digest) {
    node* n = cache.at(digest);
    remove(n);
    insert_head(n);
    return n->data;
  }

  void put(uint128_t digest, T value) {
    if (cache.size() == max_size) {
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

  void erase(uint128_t digest) {
    node* removed = cache[digest];
    cache.erase(digest);
    remove(removed);
    delete removed;
  }

  bool contains(uint128_t digest) {
    return cache.find(digest) != cache.end();
  }
  
  friend std::ostream& operator<<(std::ostream& out, const lru_cache& cache) {
    out << "FORWARDS: {";
    auto n = cache.head;
    if (n != NULL) {
      out << n->data;
      n = n->next;
      while (n != NULL) {
        out << ", " << n->data;
        n = n->next;
      }
    }
    out << "} ";
    out << "BACKWARDS: {";
    n = cache.tail;
    if (n != NULL) {
      out << n->data;
      n = n->prev;
      while (n != NULL) {
        out << ", " << n->data;
        n = n->prev;
      }
    }
    out << "}";
    return out;
  }

  typedef lru_cache_iter<T> iterator;

  iterator begin() {
    return iterator(*this, 0);
  }

  iterator end() {
    return iterator(*this, size());
  }

private:
  struct node {
    node* prev;
    node* next;
    uint128_t digest;
    T data;

    node(uint128_t digest, T data, node* prev = NULL, node* next = NULL) {
      this->digest = digest;
      this->data = data;
      this->prev = prev;
      this->next = next;
    }
  };

  void evict() {
    if (tail) {
      erase(tail->digest);
    }
  }

  void insert_head(node* n) {
    n->next = head;
    head = n;
    n->prev= NULL;
    if (!tail) {
      tail = n;
    } else {
      n->next->prev = n;
    }
  }

  void remove(node* n) {
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

  std::map<uint128_t, node*> cache;
  int max_size;
  node* head;
  node* tail;
};

template <typename T>
class lru_cache_iter {
public:
  typedef lru_cache_iter<T> iterator;

  lru_cache_iter(lru_cache<T>& cache, int size) {
    if (size == cache.size()) {
      node = NULL;
    } else {
      node = cache.head;
    }
  }

  uint128_t operator*() {
    return node->digest;
  }

  typename lru_cache<T>::node* operator->() {
    return node;
  }

  iterator & operator++() {
    node = node->next;
    return *this;
  }

  iterator operator++(int) {
    iterator clone = *this;
    ++(*this);
    return clone;
  }

  bool operator==(const lru_cache_iter<T>& other) {
    return node == other.node;
  }

  bool operator!=(const lru_cache_iter<T>& other) {
    return node != other.node;
  }

private:
  typename lru_cache<T>::node* node;
};

#endif
