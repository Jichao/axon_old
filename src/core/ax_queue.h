
#ifndef _AX_QUEUE_H_
#define _AX_QUEUE_H_

#include <cstddef>

namespace axon {

struct buf_chunk_t {
	buf_chunk_t(int n);
	~buf_chunk_t(); 
	
	int capacity;
	int nelems;
	//valid data segment
	void* data;
	buf_chunk_t *prev;
	buf_chunk_t *next;
};

struct var_msg_t {
	int length;   //length of following carry data (not include this struct)
	int type;
	char data[1];
};

//queue carry variable length data
class queue_t
{
public:
	queue_t(int n);
	~queue_t();

	int push(int n, int type, void* data);
	void pop();
	int count() { return nelems_; }
	var_msg_t* front();
	var_msg_t* back();

protected:
	//upper limit of each chunk
	static const int capacity_limit = 4000000;
	
	//disable copy construction
	queue_t (const queue_t&);
	const queue_t &operator = (const queue_t &);

protected:
	int each_capacity_;
	buf_chunk_t *chunk_spare_;
	buf_chunk_t *chunk_begin_;
	int pos_begin_;
	//queue back's chunk and pos
	buf_chunk_t *chunk_back_;
	int pos_back_; 
	//last available chunk(capacity)
	buf_chunk_t *chunk_end_;
	int pos_end_;
	int nelems_;
};


} //namespace


#endif
