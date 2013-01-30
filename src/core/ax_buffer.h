//
//Purpose: buffer data structure. 
//         convenient to manipulate raw data in engine code.

#ifndef _AX_BUFFER_H_
#define _AX_BUFFER_H_

#include "stdheader.h"

namespace axon {

#define VAR_MSG_HLEN (offsetof(var_msg_t, data))

//variable length msg in a consecutive memory block
struct var_msg_t {
	int length;   //length of following carry data ( start from data[1] )
	int type;     //make pointer conversion for "data[1]" according to type
	char data[1];  //data pointer in subsequant memory address
};

class buffer_t 
{
public:
	buffer_t();
	~buffer_t();
	buffer_t(uint32_t n);
	buffer_t(buffer_t &rhs);

	void set_shrink_limit(uint32_t n);

	int pop(uint32_t n);
	char pop_char();
	//packed in little endian
	short pop_short();
	int pop_int();

	int push(buffer_t &rhs);
	int push(const char* src, uint32_t size);
	void push(char c);
	//packed in little endian
	void push(short n);
	void push(int n);

	//expose buffer to caller to avoid memory copy
	//1. prepare space first, 
	//2. fill data to address return by tail()
	//3. flush_push() valid data size
	int prepare(uint32_t size);
	void flush_push(int actual_size);
	char* tail() { 
		if (buf_ == NULL) return NULL;
		return buf_ + end_; 
	}

	void shift();
	uint32_t len() { return len_; }
	uint32_t capacity() { return alloc_size_; }
	char* data() { 
		if (len_ <= 0 || buf_ == NULL) return NULL;
		return buf_ + start_; 
	}
	void reset(uint32_t size);
	//abandon the data, reset position pointer
	void clear();

private:
	//disable direct assignment
	buffer_t& operator=(const buffer_t &rhs);

	void alloc(uint32_t n);
	void shrink();

	char* buf_;
	uint32_t upper_limit_;   //shrink threshold
	uint32_t init_alloc_;     //capacity at the beginning

	uint32_t len_;
	uint32_t alloc_size_;   //capacity  
	uint32_t start_;  //valid data start 
	uint32_t end_;    //valid data end
	uint32_t prepare_end_;   //prepare to flush
};



} //namespace


#endif
