/**
 * index_iterator.cpp
 */
#include <cassert>

#include "index/index_iterator.h"

namespace scudb {

/*
 * NOTE: you can change the destructor/constructor method here
 * set your own input parameters
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::IndexIterator(B_PLUS_TREE_LEAF_PAGE_TYPE *leaf, int index, BufferPoolManager *bmp)
    :leaf_(leaf), index_(index), bmp_(bmp) {}

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::~IndexIterator() 
{

}

INDEX_TEMPLATE_ARGUMENTS
bool INDEXITERATOR_TYPE::isEnd()
{

}

INDEX_TEMPLATE_ARGUMENTS
const MappingType INDEXITERATOR_TYPE::operator*()
{

}

INDEX_TEMPLATE_ARGUMENTS
const MappingType INDEXITERATOR_TYPE::operator++()
{

}
template class IndexIterator<GenericKey<4>, RID, GenericComparator<4>>;
template class IndexIterator<GenericKey<8>, RID, GenericComparator<8>>;
template class IndexIterator<GenericKey<16>, RID, GenericComparator<16>>;
template class IndexIterator<GenericKey<32>, RID, GenericComparator<32>>;
template class IndexIterator<GenericKey<64>, RID, GenericComparator<64>>;

} // namespace scudb
