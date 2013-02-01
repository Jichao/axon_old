//linux epoll event driver
#if defined USE_EPOLL

#ifndef _AX_EPOLL_H_
#define _AX_EPOLL_H_

#include "stdheader.h"
#include "fd_poller.h"
#include <sys/epoll.h>

namespace axon {

class EpollPoller: public FdPollerBase
{
public:
	EpollPoller(uint32_t maxn, int timetick);
	~EpollPoller();

	virtual int add_event(int fd, int flag);
	virtual int del_event(int fd, int flag);
	virtual int do_poll(int timeout);

private:
	int epoll_fd_;
	struct epoll_event *event_list_;
};


} //namespace

#endif

#endif  //USE_KQUEUE
