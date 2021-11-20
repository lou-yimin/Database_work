/*
 * buffer_pool_manager.h
 *
 * Functionality: The simplified Buffer Manager interface allows a client to
 * new/delete pages on disk, to read a disk page into the buffer pool and pin
 * it, also to unpin a page in the buffer pool.
 */

#pragma once
#include <list>
#include <mutex>

#include "buffer/lru_replacer.h"
#include "disk/disk_manager.h"
#include "hash/extendible_hash.h"
#include "logging/log_manager.h"
#include "page/page.h"

namespace scudb {
class BufferPoolManager {
public:
  BufferPoolManager(size_t pool_size, DiskManager *disk_manager,
                          LogManager *log_manager = nullptr);

  ~BufferPoolManager();

  Page *FetchPage(page_id_t page_id);//取出页用

  bool UnpinPage(page_id_t page_id, bool is_dirty);//释放不用了

  bool FlushPage(page_id_t page_id);//写回磁盘

  Page *NewPage(page_id_t &page_id);//新建页

  bool DeletePage(page_id_t page_id);//删除页（磁盘and哈系表）

private:
  size_t pool_size_; // number of pages in buffer pool,缓冲池大小，就是extensive_hash表的大小
  Page *pages_;      // array of pages，存放的数据类型是pages,value=pages
  DiskManager *disk_manager_; //磁盘区
  LogManager *log_manager_;//日志记录
  HashTable<page_id_t, Page *> *page_table_; // to keep track of pages,可扩展哈系表
  Replacer<Page *> *replacer_;   // to find an unpinned page for replacement,LRU表，里面放哈系表中没正在被使用的page,pin>0
  std::list<Page *> *free_list_; // to find a free page for replacement,里面放哈系表中的空闲位置
  std::mutex latch_;             // to protect shared data structure
  Page* findSetPlace();
};
} // namespace scudb
