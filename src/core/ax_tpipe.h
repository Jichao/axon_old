
#ifndef _AX_TPIPE_H_
#define _AX_TPIPE_H_

#include "ax_queue.h"

namespace axon {

class tpipe_t
{
public:
	tpipe_t();
	~tpipe_t();

	void push(var_msg_t *v);
	void read(var_msg_t *v);
	int probe();

private:
	tpipe_t (const tpipt_t &);
	const tpipe_t &operator = (const tpipe_t&);
private:
	queue_t *queue;
	var_msg_t *w;
	var_msg_t *r;
	var_msg_t *c;
	AxMutex mt_;
};


}

#endif
