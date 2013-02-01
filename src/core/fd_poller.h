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
};

//fd entry status
enum FdStatus {
	FDS_EMPTY = 0,
	FDS_ACTIVE = 1,    //active fd
	FDS_WAIT_CLOSE = 2,  //peer close, waiting connect manager to close fd later
	                     // == tcp CLOSE_WAIT state
	FDS_RETIRED = 3, 	//ready to close
};

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
		uint8_t ev_mask;
		uint8_t status;  //FD status
		IFdEventReactor *reactor;
	};

public:
	FdPollerBase(uint32_t max_fds, int timetick);
	~FdPollerBase();

	int add_fd(int fd, IFdEventReactor *reactor);
	//del event poll of this fd, but close later
	int wait_close(int fd);
	int close_fd(int fd);

	//timetick for timer and polling timeout
	void set_timetick(int tm);
	int process();

	//change reactor
	int set_reactor(int fd, IFdEventReactor *reactor);

	virtual int add_event(int fd, int flag) = 0;
	virtual int del_event(int fd, int flag) = 0;
	virtual int do_poll() = 0;

	int add_timer_event(ev_timer_proc cbfunc, void* data, uint32_t timeout);
	int del_timer_event(int handle);
	uint32_t get_timepass();

protected:
	void reset_fd(int fd);

protected:
	//prealloc entry PollEntry
	PollerEntry *entry_;

	//for safety, pendingnew_/retired_ is a list to init/close fd entry after
	//event_poll iteration finished
	//there's a rare chance to occur a reuse fd conflict in a event_poll iteration
	//e.g.: close() then a new connect reuse this fd. so add to pending_new_ first
	HashMapInt *pending_new_; 
	int *retired_;  
	int retired_sz_;

	EvTimer *timer_;

	int timetick_;  //unit:ms. timetick for each poll(1000ms/server frame rate)
	uint32_t max_fds_;   //max number of fd
	uint64_t tm_last_;   //last process
};

}  //namespace

#include "kqueue.h"
#include "epoll.h"



#endif
