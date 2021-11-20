#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"

namespace scudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) {
    globalDepth = 0;
    bucketMaxSize = size;
    numBuckets = 1;
    //初始化时放一个Bucket，localDepth和globalDepth都为0
    bucketTable.push_back(std::make_shared<Bucket>(0));
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) {
    //返回key对应的Bucket的偏移量，是指针的下标，不同的下标指针可以指向相同的Bucket
  return (size_t) key;//类型强制转换
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
  return globalDepth;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  return bucketTable[bucket_id]->localDepth;
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  return numBuckets;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
    lock_guard<mutex> lockGuard(m);
    size_t hashK = HashKey(key);
    size_t pointIndex = hashK & ((1<<globalDepth)-1);
    shared_ptr<Bucket> targetBucket = bucketTable[pointIndex];
    using MapIterator = typename map<K, V>::const_iterator;
    MapIterator iter;
    for (iter = targetBucket->items.begin(); iter != targetBucket->items.end(); iter++) {
      if(iter->first == key)
      {
          value = iter->second;
          return true;
      }
    }
    return false;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
        lock_guard<mutex> lockGuard(m);
        size_t hashK = HashKey(key);
        size_t pointIndex = hashK & ((1<<globalDepth)-1);
        shared_ptr<Bucket> targetBucket = bucketTable[pointIndex];
        using MapIterator = typename map<K, V>::const_iterator;
        MapIterator iter;
        for (iter = targetBucket->items.begin(); iter != targetBucket->items.end(); iter++) {
            if(iter->first == key)
            {
                targetBucket->items.erase(iter);
                return true;
            }
        }
        return false;
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
        lock_guard<mutex> lockGuard(m);
        //取的key的后globalDepth位,能定位它应该放在哪个指针里，再看指针指的Bucket是不是满
        int pointIndex = HashKey(key) & ((1 << globalDepth)-1);
        shared_ptr<Bucket> targetBucket = bucketTable[pointIndex];
        while(targetBucket->items.size() == bucketMaxSize)//如果判断满了就得分裂，一直满一直分
        {
            //如果该Bucket只有一个指针指着,每个指针都变为原来的2倍
            //并且下标都是一个轮回，因为之后加的都是只在最高位变为1，直接按顺序push进来就好
            if (targetBucket->localDepth == globalDepth) {
                int pointNum = bucketTable.size();
                for (int i = 0; i < pointNum; i++) {
                    bucketTable.push_back(bucketTable[i]);
                }
                globalDepth++;
            }
            //无论是Bucket只有一个指针指着还是有两个指着，都要把指着targetBucket的两个指针分开
            //遍历targetBucket,把它分到两个BucketTable[]中
            shared_ptr<Bucket> zeroBucket = make_shared<Bucket>(targetBucket->localDepth + 1);
            shared_ptr<Bucket> oneBucket = make_shared<Bucket>(targetBucket->localDepth + 1);
//          map<K, V>::iterator iter;//迭代器这样会报错报错??
//          解决方法：https://blog.csdn.net/junyucsdn/article/details/50449336
            using MapIterator = typename map<K, V>::const_iterator;
            MapIterator iter;
            for (iter = targetBucket->items.begin(); iter != targetBucket->items.end(); iter++) {
                size_t itemKey = HashKey(iter->first);
                if (itemKey & (1 << targetBucket->localDepth))//最高位为1
                {
                    oneBucket->items.insert(*iter);
                } else {
                    zeroBucket->items.insert(*iter);
                }
            }
            //将targetBucket的两个指针分别指向zero和one
            for(size_t i=0; i<bucketTable.size(); i++)
            {
                if(bucketTable[i]==targetBucket)
                {
                    if(i & (1 << targetBucket->localDepth))
                    {
                        bucketTable[i]=oneBucket;
                    }
                    else
                    {
                        bucketTable[i]=zeroBucket;
                    }
                }
            }
            //重新获得pointIndex,因为globalDepth变了，要看的位数增多，放的地方也应该更准确
            numBuckets++;
            pointIndex = HashKey(key) & ((1 << globalDepth)-1);
            targetBucket = bucketTable[pointIndex];
        }
        targetBucket->items[key]=value;
//        std::cout<<"NUMBUCKET:"<<numBuckets<<" ";
}

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace scudb
