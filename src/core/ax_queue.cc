
#include "utest.h"
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
	RT_ASSERT(n < capacity_limit);
	chunk_begin_ = new buf_chunk_t(n);
	pos_begin_ = 0;
	chunk_back_ = NULL;
	pos_back_ = 0;
	chunk_end_ = chunk_begin_;
	pos_end_ = 0;
	chunk_spare_ = NULL;
	nelems_ = 0;
	each_capacity_ = n;
}

queue_t::~queue_t()
{
	buf_chunk_t *p;
	while(1) {
		if (chunk_begin_ == chunk_end_) {
			delete chunk_begin_;
			break;
		}
		p = chunk_begin_;
		chunk_begin_ = chunk_begin_->next;
		delete p;
	}
	while(chunk_spare_ != NULL)  {
		p = chunk_spare_;
		chunk_spare_ = chunk_spare_->next;	
		delete p;
	}
}

//push to prepare space before call back() to copy actual data
int queue_t::push(int n, int type, void* vdata)
{
	var_msg_t *pheader;
	buf_chunk_t *avail_chunk;
	char *cp;
	int total_len;
	int actual_n = n;
	RT_ASSERT(n > 0);
    total_len = actual_n + offsetof(var_msg_t, data);

	if (total_len > capacity_limit) return AX_RET_FULL;
	if (pos_end_ + total_len < chunk_end_ ->capacity) {
		//shift to next pointer
		cp = (char*)(chunk_end_->data);
		pheader = (var_msg_t*)(cp + pos_end_);
	} else {
		if (chunk_spare_ != NULL && chunk_spare_->capacity > total_len) {
			avail_chunk = chunk_spare_;
			chunk_spare_ = chunk_spare_->next;
			if (chunk_spare_ != NULL) chunk_spare_->prev = NULL;
		} else {
			int cap = each_capacity_;
			if (total_len > cap) cap = total_len;  //expand
			avail_chunk = new buf_chunk_t(cap);
		}
		avail_chunk->next = NULL;
		avail_chunk->prev = chunk_end_;
		chunk_end_->next = avail_chunk;
		chunk_back_= avail_chunk;
		pos_end_ = 0;
		pheader = (var_msg_t*)(chunk_end_->data);
	}
	pheader->length = actual_n;
	pheader->type = type;
	memcpy(pheader->data, vdata, n);
	chunk_back_ = chunk_end_;
	chunk_back_->nelems++;
	pos_back_ = pos_end_;
	pos_end_ += total_len;
	++nelems_ ;
	return AX_RET_OK;
}

//shift the head data after call front and processed the data
void queue_t::pop()
{
	var_msg_t *pfront;
	buf_chunk_t *pchunk;
	if (chunk_begin_->nelems == 0) return;	

	pfront = front();
	pos_begin_ += offsetof(var_msg_t, data) + pfront->length;
	chunk_begin_->nelems--;
	--nelems_;

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
	char* p = (char*)(chunk_begin_->data);
	return (var_msg_t*)(p + pos_begin_);
}

var_msg_t* queue_t::back()
{
	char* p;
	if (NULL == chunk_back_) return NULL; 
   	p = (char*)(chunk_back_->data);
	return (var_msg_t*)(p + pos_back_);
}

UTEST(queue_t)
{
	char word[] = "hello,world!";
	var_msg_t *p;
	queue_t g(100);

	g.push(sizeof(word), 1, (void*)word);
	p = g.back();
	printf("%s", (char*)(p->data));
	
	g.push(sizeof(word), 1, (void*)word);

}

} //namespace
