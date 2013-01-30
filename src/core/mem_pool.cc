//simple memory pool

#include "ax_debug.h"
#include "utest.h"
#include "mem_pool.h"

namespace axon {

#define POOL_ALLOC_FLAG 0x77
#define POOL_FREE_FLAG  0x0

// use one more byte in the tail of each node to identify free node or allocated node
// (this byte help to check memory boundary when memory bug occur) 
MemPool::MemPool(uint32_t n, uint32_t elt_size) 
{
	if (elt_size < sizeof(int) + 1) elt_size = sizeof(int) + 1;
	elt_size_ = elt_size;
	offset_ = elt_size_ + 1;  //need one more byte

	buf_ = (char*)malloc(n * offset_);
	RT_ASSERT(buf_ != NULL);
	memset(buf_, 0, n*offset_);

	buf_end_ = buf_ + n * offset_;
	free_node_ = 0;
	capacity_ = n;
	nalloc_ = 0;
	//init free link
	for(int i=0; i<capacity_-1; i++) {
		*(int*)(buf_ + i*offset_) = i+1;
	}
	*(int*)(buf_ + (capacity_-1)*offset_) = -1;
}

MemPool::~MemPool()
{
	::free(buf_);
}

//alloc in memory pool. if the pool is full, use normal malloc
//the pool size should large enough
void* MemPool::alloc()
{
	char* p;
	if (nalloc_ >= capacity_) return malloc(elt_size_);

	RT_ASSERT(free_node_ >= 0);
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
	if (pc < buf_ || pc > buf_end_) {
		::free(p);
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
	struct s_TT {
		int a;
		int b;
	};
	const int psize = 3;
	MemPool pool(psize, sizeof(s_TT));

	s_TT *p = (s_TT*)pool.alloc();
	s_TT *arr[100];

	p->a = 1000;
	arr[0] = (s_TT*)pool.alloc();

	UT_ASSERT(pool.size() == 2 && pool.capacity() == psize);
	pool.free(p);
	UT_ASSERT(pool.size() == 1);

	arr[1] = (s_TT*)pool.alloc();
	//check free_node link
	UT_LOGV("arr[1]=%p p=%p", arr[1], p);
	UT_ASSERT(arr[1] == p);
	pool.free(arr[0]);
	pool.free(arr[1]);

	for(int i=0; i<pool.capacity(); i++) {
		arr[i] = (s_TT*)pool.alloc();
	}
	arr[psize] = (s_TT*)pool.alloc();
	pool.free(arr[psize]);
}

} //namespace
