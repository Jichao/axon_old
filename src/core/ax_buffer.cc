//common buffer data sturcture

#include "ax_buffer.h"
#include "ax_debug.h"
#include "utest.h"

namespace axon {

buffer_t::buffer_t() : buf_(NULL), len_(0), alloc_size_(0), start_(0), end_(0)
{
	upper_limit_ = 0;
	init_alloc_ = 0;
}

buffer_t::buffer_t(uint32_t n)
{
	alloc(n);
	upper_limit_ = n * 4;  //4 times by default
	init_alloc_ = n;
}

buffer_t::buffer_t(buffer_t &rhs)
{
	alloc(rhs.capacity());
	push(rhs);
	upper_limit_ = alloc_size_ * 4;
	init_alloc_ = alloc_size_;
}

buffer_t::~buffer_t()
{
	free(buf_);
}

void buffer_t::alloc(uint32_t n)
{
	buf_ = (char*)malloc(n);
	RT_ASSERT(buf_ != NULL);
	start_ = len_ = end_ = 0;
	alloc_size_ = n;
}

void buffer_t::shrink()
{
	//sometimes the buffer will expand much larger than init size,
	//because consumer thread/process stuck/block
	//shrink to save memory after summit.
	char* newp = (char*)malloc(init_alloc_);
	RT_ASSERT(newp != NULL);
	if (len_ > 0) memcpy(newp, buf_ + start_, len_);
	free(buf_);
	buf_ = newp;
	alloc_size_ = init_alloc_;
}

void buffer_t::shift()
{
	if (buf_ == NULL) return;
	if (upper_limit_ > 0 && alloc_size_ > upper_limit_ && len_ < init_alloc_) {
		shrink();
	} else {
		memmove(buf_, buf_ + start_, len_);
	}
	start_ = 0;
	end_ = len_;
}

//pop data in front
//return:  len after pop
int buffer_t::pop(uint32_t n)
{
	uint32_t len = len_;
	if (n >= len) {
		//pop all
		start_ = 0;
		end_ = 0;
		len_ = 0;
		if (upper_limit_ > 0 && alloc_size_ > upper_limit_) {
			shrink();
		}
		return len;
	}
	start_ += n;
	len_ -= n;
	return len_;
}

char buffer_t::pop_char()
{
	char* p = data();	
	RT_ASSERT(p != NULL);
	pop(sizeof(char));
	return *p;
}

//little endian
//Warning: if the buffer will send to remote server, make sure the byte order is little endian(like x86).
//        Don't use this function when communicate with different kind of machines.
short buffer_t::pop_short()
{
	short* p = (short*)data();
	RT_ASSERT(p != NULL);
	pop(sizeof(short));
	return *p;
}

int buffer_t::pop_int()
{
	int* p = (int*)data();
	RT_ASSERT(p != NULL);
	pop(sizeof(int));
	return *p;
}

//push another buffer
int buffer_t::push(buffer_t &rhs)
{
	return push(rhs.data(), rhs.len());
}

//prepare space
int buffer_t::prepare(uint32_t size)
{
	uint32_t need;
	char *newp;
	need = len_ + size;
	//shift first
	if (end_ + size > alloc_size_) shift();
	if (need > alloc_size_ ) {
		alloc_size_ = start_ + need;
		newp = (char*)realloc(buf_, alloc_size_);
		RT_ASSERT(newp != NULL);
		buf_ = newp;
	}
	return alloc_size_;
}

//push_back with no data. just move the end_
void buffer_t::flush_push(int actual_size)
{
	//after calling prepare, the mem space should be enough
	RT_ASSERT(end_ + actual_size <= alloc_size_);
	len_ += actual_size;
	end_ += actual_size;	
}

//push in the end
int buffer_t::push(const char* src, uint32_t size)
{
	prepare(size);
	RT_ASSERT(buf_ != NULL);
	memcpy(buf_ + end_, src, size);
	len_ += size;
	end_ += size;
	return len_;
}

void buffer_t::push(char c) 
{
	push((char*)&c, (uint32_t)sizeof(char));
}

//little endian in x86 machine
//Warning: if the buffer will send to remote server, make sure the byte order is little endian(like x86).
//        Don't use this function when communicate with different kind of machines.
void buffer_t::push(short n) 
{
	push((char*)&n, (uint32_t)sizeof(short));
}

void buffer_t::push(int n)
{
	push((char*)&n, (uint32_t)sizeof(int));
}

//resize the buffer(keep the data)
void buffer_t::reset(uint32_t size)
{
	char *newp;
	
	if (size != alloc_size_) {
		alloc_size_ = size;
		newp = (char*)realloc(buf_, alloc_size_);
		RT_ASSERT(newp != NULL);
		buf_ = newp;
	} else if (size == 0) {
		alloc_size_ = 0;
		free(buf_);
		buf_ = NULL;
	} else {
		//same size, do not realloc
	}

	clear();
}

void buffer_t::clear()
{
	start_ = end_ = len_ = 0;
}

void buffer_t::set_shrink_limit(uint32_t n)
{
	upper_limit_ = n;
}

UTEST(buffer_t)
{
	char srcbuf[] = "abcdefghijklmnopqrstuvwxyz";
	char* p;
	buffer_t buf1(100);
	buffer_t buf2;
	buffer_t buf3(50);

	buf1.push(srcbuf, 10);
	p = buf1.data();
	UT_ASSERT(p[1] == 'b');
	buf1.pop(3);
	p = buf1.data();
	UT_ASSERT(p[0] == 'd');
	buf1.pop(100);
	//self expand
	buf2.push(srcbuf, 10);
	p = buf2.data();
	UT_ASSERT(p[2] == 'c');
	//expand and shrink
	int uplimit = 1000;
	buf3.set_shrink_limit(uplimit);
	for(int i=0; i<200; i++) {
		buf3.push(srcbuf, 10);
	}
	UT_ASSERT(buf3.capacity() > uplimit);
	for(int i=0; i<200; i++) {
		buf3.pop(10);
	}
	UT_LOGV("before shift: cap %d len:%d", buf3.capacity(), buf3.len());
	//do a shift, then will shrink
	for(int i=0; i<10; i++) {
		buf3.push(srcbuf, 10);
	}
	UT_LOGV("buf capacity: %d len:%d", buf3.capacity(), buf3.len());
	UT_ASSERT(buf3.capacity() < uplimit);
}


} //namespace
