//
//Purpose: simple memory pool
//Created: Daly 2012-11-18

#ifndef _AX_MEM_POOL_H_
#define _AX_MEM_POOL_H_

#include "stdheader.h"

namespace axon {

//This pool won't auto expand, if the pool is full, use system malloc instead
//So user should init suitable size for the pool(large enough in practical situation)
class MemPool
{
public:
	MemPool(uint32_t n, uint32_t elt_size);
	~MemPool();
	void* alloc();
	void free(void* p);
	int size() const { return nalloc_; }
	int capacity() const { return capacity_; }

private:
	char* buf_;
	char* buf_end_;
	int free_node_;    //free node link head( next Nth node instead of next pointer)
	int capacity_;
	int nalloc_;
	int elt_size_;  //valid data size of each node
	int offset_;    //actual size of each node
};



} //namespace

#endif
