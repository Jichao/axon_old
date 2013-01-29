
#include "ax_thread.h"
#include <signal.h>

namespace axon {

extern "C" void *thread_routine (void *arg)
{
	AxThread *self = (AxThread*)arg;
	self->wfn_ ( self->arg_ );
	return NULL;
}


int AxThread::start(thread_worker_fn *wfn, void *arg)
{
	wfn_ = wfn;
	arg_ = arg;
	int ret = pthread_create (&handle_, NULL, thread_routine, this);
	return ret;
}

int AxThread::stop()
{
	int ret = pthread_join(handle_, NULL);
	return ret;
}




} //namespace
