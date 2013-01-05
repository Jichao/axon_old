//
//fd event poller base class

#ifndef _AX_FD_POLLER_H_
#define _AX_FD_POLLER_H_

#include "stdheader.h"
#include "common_def.h"
#include "ax_hashmap.h"
#include "ax_timer.h"

namespace axon {

//poller according to OS
#if defined USE_SELECT
	typedef SelectPoller EvPoller;
#elif defined USE_KQUEUE
	class KqueuePoller;
	typedef KqueuePoller EvPoller;
#elif defined USE_EPOLL
	class EpollPoller;
	typedef EpollPoller EvPoller;
#endif

enum EvFlag {
	EV_NULL = 0,
	EV_READ = 1,
	EV_WRITE = 2,
	EV_INVALID = 4,
};

typedef uint32_t ev_handle_t;

//reactor class should derived from this interface 
class IFdEventReactor
{
public:
	virtual void on_ev_read(int fd) = 0;
	virtual void on_ev_write(int fd) = 0;
};


//base class of all kinds of poller
//basic interface to fd event caller
class FdPollerBase
{
public:
	struct PollerEntry
	{
		int fd;
		uint32_t handle;
		int ev_mask;
		IFdEventReactor *reactor;
	};

public:
	FdPollerBase(uint32_t max_fds, int timetick);
	~FdPollerBase();

	ev_handle_t add_fd(int fd, IFdEventReactor *reactor);
	int rm_fd(int fd);
	int set_reactor(int fd, uint32_t h, IFdEventReactor *reactor);
	void set_timetick(int tm);
	PollerEntry* find_entry(int fd, uint32_t h);
	int process();

	virtual int add_event(int fd, uint32_t h, int flag) = 0;
	virtual int del_event(int fd, uint32_t h, int flag) = 0;
	virtual int do_poll(int timeout) = 0;
	int add_timer_event(ev_timer_proc cbfunc, void* data, uint32_t timeout);
	int del_timer_event(int handle);

protected:
	HashMapInt *fdmap_;
	EvTimer *timer_;
	int timetick_;  //unit:ms. timetick for each poll(1000ms/server frame rate)
	ev_handle_t handle_;  //event handle
	uint32_t max_fds_;   //max number of fd
};

}  //namespace

#include "kqueue.h"
#include "epoll.h"



#endif
