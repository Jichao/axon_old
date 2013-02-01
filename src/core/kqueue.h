
#if defined USE_KQUEUE

#ifndef _AX_KQUEUE_H_
#define _AX_KQUEUE_H_

#include "stdheader.h"
#include "fd_poller.h"
#include <sys/event.h>

namespace axon {

class KqueuePoller : public FdPollerBase
{
public:
	KqueuePoller(uint32_t maxn, int timetick);
	~KqueuePoller();

	virtual int add_event(int fd, int flag);
	virtual int del_event(int fd, int flag);
	virtual int do_poll();

private:
	int kqueue_fd_;
	struct kevent *event_list_;
};


} //namespace

#endif

#endif  //USE_KQUEUE
