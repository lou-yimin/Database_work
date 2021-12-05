/*
 * extendible_hash.h : implementation of in-memory hash table using extendible
 * hashing
 *
 * Functionality: The buffer pool manager must maintain a page table（页表） to be able
 * to quickly map a PageId to its corresponding memory location; or alternately
 * report that the PageId does not match any currently-buffered page.
 */

#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include<map>
#include<memory>
#include<mutex>
#include "hash/hash_table.h"
using namespace std;
namespace scudb {

template <typename K, typename V>
class ExtendibleHash : public HashTable<K, V> {
public:
  // constructor
ExtendibleHash(size_t size);
  // helper function to generate hash addressing；返回K应该存放的bucket的偏移量（存在哪个bucket）
  size_t HashKey(const K &key);
  // helper function to get global & local depth
  int GetGlobalDepth() const;//返回目前全部哈系表的深度
  int GetLocalDepth(int bucket_id) const;//返回目前bucket_id在哈系表中的深度
  int GetNumBuckets() const;//返回哈系表中分配的bucket总数
  // lookup and modifier
  bool Find(const K &key, V &value) override;//key若在哈系表中，把key的指针存在value中，并返回ture
  bool Remove(const K &key) override;//在哈系表中移除key的键值对(key/value pair)并返回true
  void Insert(const K &key, const V &value) override;//插入key/value,如果key已存在，则用value重写

private:
  // add your own member variables here
  class Bucket{//Bucket
  public:
      int localDepth;
      map<K, V> items;
      explicit Bucket(int depth):localDepth(depth) {};
  };
    int globalDepth;
    size_t bucketMaxSize;//每一个Bucket最大能容纳的items数量
    int numBuckets;//Bucket的总数
    vector<shared_ptr<Bucket>> bucketTable;//指向Bucket的智能指针组
    mutex m;
};
} // namespace scudb
