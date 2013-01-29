//thread and mutex wrapper (in order to cross platform if needed)
//

#ifndef _AX_THREAD_H_
#define _AX_THREAD_H_

#include <pthread.h>
#include "ax_debug.h"

namespace axon {


typedef void (thread_worker_fn) (void*);

class AxThread
{
public:
	AxThread() {
		arg_ = NULL;
		wfn_ = NULL;
	}

	int start (thread_worker_fn *wfn, void *arg);
	//wait for thread exit
	int stop();

	thread_worker_fn *wfn_;
	void *arg_;
	pthread_t handle_;
private:
	AxThread (const AxThread &);
	const AxThread &operator=(const AxThread &);
};

class AxMutex
{
public:
	inline AxMutex()  {
		int ret = pthread_mutex_init(&mutex_, NULL);
		RT_ASSERT(ret == 0);
	}

	inline ~AxMutex() {
		int ret = pthread_mutex_destroy (&mutex_);
		RT_ASSERT(ret == 0);
	}

	inline int lock() {
		int ret = pthread_mutex_lock (&mutex_);
		return ret;
	}

	inline int unlock() {
		int ret = pthread_mutex_unlock (&mutex_);
		return ret;
	}
private:
	//disable copy consturctor
	AxMutex(const AxMutex &);
	const AxMutex &operator = (const AxMutex &);

private:
	pthread_mutex_t mutex_;
	
};


} //namespace

#endif
