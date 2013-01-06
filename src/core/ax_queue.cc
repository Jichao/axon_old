
#include "ax_debug.h"
#include "ax_queue.h"

namespace axon {

buf_chunk_t::buf_chunk_t(int n)
{
	data = malloc(n);
	RT_ASSERT(data != NULL);
	capacity = n;
	prev = next = NULL;
	nelems = 0;
}

buf_chunk_t::~buf_chunk_t()
{
	free(data);
}

queue_t::queue_t(int n)
{
	chunk_begin_ = new buf_chunk_t(n);
	pos_begin_ = 0;
	chunk_back_ = NULL;
	pos_back_ = 0;
	chunk_end_ = chunk_begin_;
	pos_end_ = 0;
	chunk_spare_ = NULL;
}

queue_t::~queue_t()
{
	while(1) {
		if (chunk_begin_ == chunk_end_) {
			delete chunk_begin_;
			break;
		}
		buf_chunk_t *p = chunk_begin_;
		chunk_begin_ = chunk_begin_->next;
		delete p;
	}
}

//push after call front to copy actual data
//use a int as message header
void queue_t::push(int n)
{
	var_msg_t *pheader;
	buf_chunk_t *avail_chunk;
	char *cp;
	if (pos_end_ + n + sizeof(var_msg_t) < chunk_end_ ->capacity) {
		//shift to next pointer
		cp = (char*)chunk_end_;
		pheader = (var_msg_t*)(cp + pos_end_);
	} else {
		if (chunk_spare_ != NULL) {
			avail_chunk = chunk_spare_;
			chunk_spare_ = chunk_spare_->next;
			if (chunk_spare_ != NULL) chunk_spare_->prev = NULL;
		} else {
			avail_chunk = new buf_chunk_t(chunk_end_->capacity);
		}
		avail_chunk->next = NULL;
		avail_chunk->prev = chunk_end_;
		chunk_end_->next = avail_chunk;
		chunk_back_= avail_chunk;
		pos_end_ = 0;
		pheader = (var_msg_t*)chunk_end_;
	}
	pheader->length = n;
	pheader->type = 0;
	chunk_back_ = chunk_end_;
	chunk_back_->nelems++;
	pos_back_ = pos_end_;
	pos_end_ += sizeof(var_msg_t) + n;
}

//shift the head data after call front and processed the data
void queue_t::pop()
{
	var_msg_t *pfront;
	buf_chunk_t *pchunk;
	if (chunk_begin_->nelems == 0) return;	

	pfront = front();
	pos_begin_ += sizeof(var_msg_t) + pfront->length;
	chunk_begin_->nelems--;

	if (chunk_begin_->nelems == 0) {
		//this chunk become empty
		if (chunk_back_ == chunk_begin_) {
			//no more data in the queue, reset this chunk
			pos_back_ = 0;
			chunk_back_ = NULL;
			RT_ASSERT(chunk_end_ == chunk_begin_);
			pos_end_ = 0;
		} else {
			pchunk = chunk_begin_;
			//swap to spare chunk
			if (chunk_spare_ != NULL) {
				chunk_spare_->prev = pchunk;
			}
			pchunk->next = chunk_spare_;
			pchunk->prev = NULL;	

			chunk_begin_ = chunk_begin_->next;
			chunk_begin_->prev = NULL;
		}
		pos_begin_ = 0;
	}
}

var_msg_t* queue_t::front()
{
	char* p = (char*)chunk_begin_;
	return (var_msg_t*)(p + pos_begin_ + sizeof(int));
}

var_msg_t* queue_t::back()
{
	char* p;
	if (NULL == chunk_back_) return NULL; 
   	p = (char*)chunk_back_;
	return (var_msg_t*)(p + pos_back_ + sizeof(int));
}


} //namespace
