//simple memory pool

#include "ax_debug.h"
#include "utest.h"
#include "mem_pool.h"

namespace axon {

#define POOL_ALLOC_FLAG 0x11
#define POOL_FREE_FLAG  0x77

// use one more byte in the tail of each node to identify free node or allocated node
// (this byte help to check memory boundary when memory bug occur) 
MemPool::MemPool(uint32_t n, uint32_t elt_size) 
{
	if (elt_size < sizeof(int) + 1) elt_size = sizeof(int) + 1;
	buf_ = (char*)malloc(n * offset_);
	RT_ASSERT(buf_ != NULL);

	elt_size_ = elt_size;
	offset_ = elt_size_ + 1;  //need one more byte
	buf_end_ = buf_ + n * offset_;
	free_node_ = 0;
	capacity_ = n;
	nalloc_ = 0;
}

MemPool::~MemPool()
{
	free(buf_);
}

//alloc in memory pool. if the pool is full, use normal malloc
//the pool size should large enough
void* MemPool::alloc()
{
	char* p;
	if (nalloc_ >= capacity_) return malloc(elt_size_);

   	p = buf_ + free_node_ * offset_;
	free_node_ = *(int*)p;
	//verify node. if false, there're memory segment fault
	RT_ASSERT(*(p + elt_size_) == POOL_FREE_FLAG); 
	*(p + elt_size_) = POOL_ALLOC_FLAG;   //special byte flag
	++nalloc_;
	return (void*)p;
}

void MemPool::free(void* p)
{
	char* pc = (char*)p;
	int* pi;
	int n;
	if (p < buf_ || p > buf_end_) {
		free(p);
		return;
	}
	//check memory boundary
	RT_ASSERT(*(pc + elt_size_) == POOL_ALLOC_FLAG);
	pi = (int*)p;
    n = (int)(pc - buf_) / offset_;
	*pi = free_node_;
	*(pc + elt_size_) = POOL_FREE_FLAG;  //special byte flag
	free_node_ = n;
	--nalloc_;
}

UTEST(MemPool)
{
	MemPool pool(100, sizeof(int));
	int *p = (int*)pool.alloc();
	*p = 1000;
	pool.free(p);

}

} //namespace
