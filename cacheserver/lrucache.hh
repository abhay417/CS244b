#ifndef __LRU_CACHE_H__
#define __LRU_CACHE_H__

#include <iostream>
#include <cstring>
#include <unordered_map>
#include "include/server.hh"

// All defined here to avoid template shenanigans

template <typename T>
class lru_cache {
public:
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

  void set_max_size(int size) {
    max_size = size;
  }

  T get(std::string key) {
    node* n = cache.at(key);
    remove(n);
    insert_head(n);
    return n->data;
  }

  void put(std::string key, T value) {
    if (cache.size() == max_size) {
      evict();
    }
    std::cout << cache.size() << " " << max_size << std::endl;
    node* n;
    if (contains(key)) {
      n = cache[key];
      remove(n);
    } else {
      n = new node(key, value);
      cache[key] = n;
    }
    insert_head(n);
  }

  bool contains(std::string key) {
    return cache.find(key) != cache.end();
  }
  
  friend std::ostream& operator<<(std::ostream& out, const lru_cache& cache) {
    out << "{";
    auto n = cache.head;
    if (n != NULL) {
      out << n->data;
      n = n->next;
      while (n != NULL) {
        out << ", " << n->data;
        n = n->next;
      }
    }
    out << "}";
    out << "{";
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

private:
  struct node {
    node* prev;
    node* next;
    std::string key;
    T data;

    node(std::string key, T data, node* prev = NULL, node* next = NULL) {
      this->key = key;
      this->data = data;
      this->prev = prev;
      this->next = next;
    }
  };

  void evict() {
    std::cout << "EVICT" << std::endl;
    if (tail) {
      node* evicted = tail;
      tail = tail->prev;
      cache.erase(evicted->key);
      delete evicted;
      if (tail) {
        tail->next = NULL;
      } else {
        head = NULL;
      }
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

  std::unordered_map<std::string, node*> cache;
  int max_size;
  node* head;
  node* tail;
};

#endif
