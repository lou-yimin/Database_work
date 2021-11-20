#include "buffer/buffer_pool_manager.h"

namespace scudb {

/*
 * BufferPoolManager Constructor
 * When log_manager is nullptr, logging is disabled (for test purpose)
 * WARNING: Do Not Edit This Function
 */
BufferPoolManager::BufferPoolManager(size_t pool_size,
                                                 DiskManager *disk_manager,
                                                 LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager),
      log_manager_(log_manager) {
  // a consecutive memory space for buffer pool
  pages_ = new Page[pool_size_];
  page_table_ = new ExtendibleHash<page_id_t, Page *>(BUCKET_SIZE);
  replacer_ = new LRUReplacer<Page *>;
  free_list_ = new std::list<Page *>;

  // put all the pages into free list
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_->push_back(&pages_[i]);
  }
}

/*
 * BufferPoolManager Deconstructor
 * WARNING: Do Not Edit This Function
 */
BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete page_table_;
  delete replacer_;
  delete free_list_;
}

/**
 * 1. search hash table.
 *  1.1 if exist, pin the page and return immediately
 *  1.2 if no exist, find a replacement entry from either free list or lru
 *      replacer. (NOTE: always find from free list first)
 * 2. If the entry chosen for replacement is dirty, write it back to disk.
 * 3. Delete the entry for the old page from the hash table and insert an
 * entry for the new page.
 * 4. Update page metadata, read page content from disk file and return page
 * pointer
 */
Page *BufferPoolManager::FetchPage(page_id_t page_id) {
    std::lock_guard<std::mutex> guard(latch_);
    Page *fPage = nullptr;
    //如果在哈系表中存在，直接返回page
    if(page_table_->Find(page_id,fPage))
    {
        if(fPage->pin_count_==0)
        {
            replacer_->Erase(fPage);//此时有新线程访问，从LRU中删除
        }
        fPage->pin_count_++;
        return fPage;
    }
    else//不在哈系表中，则需要在哈系表中找空位 或 替换LRU中能替换的，从磁盘拉上来存放
    {
        fPage = findSetPlace();
        if(fPage == nullptr)//哈系表满且LRU中没有可替换的时，return nullptr
        {
            return nullptr;
        }
        else//有位置，则从磁盘中取出，放在fPage
        {
            disk_manager_->ReadPage(page_id, fPage->GetData());//从磁盘读出来
            fPage->page_id_ = page_id;
            fPage->pin_count_ = 1;
            page_table_->Insert(page_id, fPage);//,page_id的已在findSetPlace()中写回且清空，直接放哈系表里
            return fPage;
        }
    }
}

/**
 * Implementation of unpin page
 * if pin_count>0, decrement it and if it becomes zero, put it back to
 * replacer if pin_count<=0 before this call, return false. is_dirty: set the
 * dirty flag of this page
 */
bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty) {
    std::lock_guard<std::mutex> guard(latch_);
    Page *page = nullptr;
    if(page_table_->Find(page_id,page))//若找到了
    {
        if(page->pin_count_<=0)//if pin_count<=0 before this call, return false
        {
            return false;
        }
        else
        {//if pin_count>0, decrement it and if it becomes zero, put it back to* replacer
            page->pin_count_--;
            if(page->pin_count_==0)
            {
                replacer_->Insert(page);
            }
            if(is_dirty)//is_dirty: set the dirty flag of this page
            {
                page->is_dirty_ = true;
            }
            return true;
        }
    }
    else//没在哈系表中找到
    {
        return false;
    }
}

/**
 * Used to flush a particular page of the buffer pool to disk. Should call the
 * write_page method of the disk manager
 * if page is not found in page table, return false
 * NOTE: make sure page_id != INVALID_PAGE_ID
 */
bool BufferPoolManager::FlushPage(page_id_t page_id) {
    std::lock_guard<std::mutex> guard(latch_);
    Page* page = nullptr;
    if(!page_table_->Find(page_id,page))//哈希表中没找到
    {
        return false;
    }
    else//找到则仅写回磁盘，LRU与Free不修改
    {
        disk_manager_->WritePage(page_id, page->GetData());
        page->is_dirty_ = false;
        return true;
    }
}

/**
 * User should call this method for deleting a page. This routine will call
 * disk manager to deallocate the page. First, if page is found within page
 * table, buffer pool manager should be reponsible for removing this entry out
 * of page table, reseting page metadata and adding back to free list. Second,
 * call disk manager's DeallocatePage() method to delete from disk file. If
 * the page is found within page table, but pin_count != 0, return false
 */
bool BufferPoolManager::DeletePage(page_id_t page_id) {
    std::lock_guard<std::mutex> guard(latch_);
    Page* page = nullptr;
    if(page_table_->Find(page_id,page))
    {
        if(page->pin_count_>0)//找到但是有其他线程访问
        {
            return false;
        }
        else//找到且无线程访问，可删除
        {
            //哈系表清空这个page
            replacer_->Erase(page);//从LRU中删掉
            page_table_->Remove(page_id);//从哈系表中取出
            //初始化page参数，放入free
            page->page_id_ = INVALID_PAGE_ID;//INVALID_PAGE_ID=-1
            page->is_dirty_ = false;
            page->ResetMemory();
            free_list_->push_back(page);
            //磁盘清空
            disk_manager_->DeallocatePage(page_id);
            return true;
        }
    }
    else//不在哈系表中，直接清理磁盘
    {
        disk_manager_->DeallocatePage(page_id);
        return true;
    }
}

/**
 * User should call this method if needs to create a new page. This routine
 * will call disk manager to allocate a page.
 * Buffer pool manager should be responsible to choose a victim page either
 * from free list or lru replacer(NOTE: always choose from free list first),
 * update new page's metadata, zero out memory and add corresponding entry
 * into page table. return nullptr if all the pages in pool are pinned
 */
Page *BufferPoolManager::NewPage(page_id_t &page_id) {
    std::lock_guard<std::mutex> guard(latch_);
    Page* page = nullptr;
    page = findSetPlace();
    if(page == nullptr)//没位置
    {
        return nullptr;
    }
    else
    {
        // 有能用的page，在磁盘中也得分配空间
        page_id = disk_manager_->AllocatePage();
        //分配哈系表Page
        page->page_id_ = page_id;
        page->is_dirty_ = true;//有Pin,变脏
        page->pin_count_ = 1;
        page_table_->Insert(page->page_id_, page);
        return page;
    }
}
/**
 * 找到从磁盘中拉上来的page的存放位置，
 * 若free_list中有位置，直接随便放里面，
 * 若freeList无位置，从LRU中找到被替换的那个，脏则写磁盘，清位置
 * 若LRU无元素，返回nullptr，无位置
 */
Page* BufferPoolManager::findSetPlace()
{
    Page *page;
    if(!free_list_->empty())//哈系表中有空位
    {
        page = free_list_->front();//返回front位置给page
        free_list_->pop_front();//从List中删除了
        return page;
    }
    else if(replacer_->Victim(page))//哈系表无空位，但LRU中有可替换的
    {
        if (page->is_dirty_) {//若脏则写回
            disk_manager_->WritePage(page->page_id_, page->GetData());
            page->is_dirty_ = false;
        }
        page_table_->Remove(page->page_id_);//从table中移除

        page->ResetMemory();//空出位置
        page->page_id_ = INVALID_PAGE_ID;//INVALID_PAGE_ID=-1
        return  page;
    }//根本没位置
    else//replacer_->Victim(page)=nullptr
    {
        return nullptr;
    }
}
} // namespace scudb

