//common buffer class

#include "ax_buffer.h"
#include "ax_debug.h"
#include "utest.h"

namespace axon {

buffer_t::buffer_t() : buf_(NULL), len_(0), alloc_size_(0), start_(0), end_(0)
{

}

buffer_t::buffer_t(uint32_t n)
{
	alloc(n);
}

buffer_t::buffer_t(buffer_t &rhs)
{
	alloc(rhs.capacity());
	push(rhs);
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

void buffer_t::shift()
{
	if (buf_ == NULL) return;
	memmove(buf_, buf_ + start_, len_);
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

int buffer_t::push(buffer_t &rhs)
{
	return push(rhs.data(), rhs.len());
}

//push in the end
int buffer_t::push(const char* src, uint32_t size)
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

//resize the buffer
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

UTEST(buffer_t)
{
	char srcbuf[] = "abcdefghijklmnopqrstuvwxyz";
	char* p;
	buffer_t buf1(100);
	buffer_t buf2;

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
}


} //namespace
