/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"
#include <map>
namespace scudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
    if(LRUList.empty())
    {
        LRUList.push_back(value);
    }
    else
    {//此处修改了Bug,STL中迭代器与erase的陷阱:https://www.cnblogs.com/blueoverflow/p/4923523.html
        for(typename list<T>::iterator iter = LRUList.begin();iter!=LRUList.end();)
        {
            if(value == *iter)
            {
                LRUList.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
        LRUList.push_back(value);
    }
}


/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
    if(LRUList.empty())
    {
        return false;
    }
    else
    {
        value = LRUList.front();
        LRUList.pop_front();
        return true;
    }
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
    bool findValue = false;
    for(typename list<T>::iterator iter = LRUList.begin();iter!=LRUList.end();)
    {
        if(value == *iter)
        {
            findValue = true;
            LRUList.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
    return findValue;
}

template <typename T> size_t LRUReplacer<T>::Size() {
    //List中本身就不会产生重复元素，不必判断，想复杂了
//    int size = LRUList.size();
//    map<T, bool> myMap;
//    typename list<T>::iterator iter = LRUList.begin();
//    for(iter = LRUList.begin();iter!=LRUList.end();iter++)
//    {
//        if(myMap[*iter] == true)
//        {
//         size--;
//        }
//        else
//        {
//            myMap[*iter] = true;
//        }
//    }
    return LRUList.size();
}

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace scudb
