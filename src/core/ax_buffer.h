//
//Purpose: buffer data structure. 
//         convenient to manipulate raw data in engine code.
//Created: Daly 2012-11-18
//

#ifndef _AX_BUFFER_H_
#define _AX_BUFFER_H_

#include "stdheader.h"

namespace axon {

class buffer_t 
{
public:
	buffer_t();
	~buffer_t();
	buffer_t(uint32_t n);
	buffer_t(buffer_t &rhs);

	void alloc(uint32_t n);
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


	void shift();
	uint32_t len() { return len_; }
	uint32_t capacity() { return alloc_size_; }
	char* data() { return buf_ + start_; }
	void reset(uint32_t size);
	//abandon the data, reset position pointer
	void clear();

private:
	//disable direct assignment
	buffer_t& operator=(const buffer_t &rhs);

	char* buf_;
	uint32_t len_;
	uint32_t alloc_size_;   //capacity  
	uint32_t start_;  //valid data start 
	uint32_t end_;    //valid data end
};



} //namespace


#endif
