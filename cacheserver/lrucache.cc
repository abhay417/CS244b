#include <cstring>
#include "lrucache.hh"
#include "include/server.hh"

lru_cache::lru_cache(int max_size) 
{
  this->max_size = max_size;
  head = NULL;
  tail = NULL;
}

lru_cache::~lru_cache() 
{
  while (head != NULL) {
    node* next = head->next;
    delete head;
    head = next;
  }
}

cache_data
lru_cache::get(std::string key) 
{
  node* n = cache.at(key);
  remove(n);
  insert_head(n);
  return n->data;
}

void
lru_cache::set_max_size(int size)
{
  max_size = size;
}

void
lru_cache::put(std::string key, cache_data value)
{
  if (cache.size() == max_size) {
    evict();
  }
  node* n;
  if (contains(key)) {
    n = cache[key];
    remove(n);
  } else {
    n = new node(value);
  }
  insert_head(n);
}

bool 
lru_cache::contains(std::string key) 
{
  return cache.find(key) != cache.end();
}

void 
lru_cache::evict() 
{
  if (tail) {
    node* evicted = tail;
    tail = tail->prev;
    delete evicted;
    if (tail) {
      tail->next = NULL;
    } else {
      head = NULL;
    }
  }
}

void 
lru_cache::remove(node* n) 
{
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

void
lru_cache::insert_head(node* n)
{
  n->next = head;
  head = n;
  n->prev = NULL;
  if (!tail) {
    tail = n;
  }
}
