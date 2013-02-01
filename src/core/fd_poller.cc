
#include "ax_debug.h"
#include "fd_poller.h"
#include "common_def.h"

namespace axon {

FdPollerBase::FdPollerBase(uint32_t maxn, int timetick) : timetick_(timetick_)
{
	timer_ = new EvTimer(timetick_);
	pending_new_ = new HashMapInt(10);
	retired_ = new int[maxn];   
	memset(retired_, 0, sizeof(int)*maxn);
	retired_sz_ = 0;

	entry_ = new PollerEntry[maxn];
	RT_ASSERT(entry_ != NULL);
	max_fds_ = maxn;
	tm_last_ = 0;

	for(int i=0; i<(int)maxn; i++) {
		reset_fd(i);
	}
}

FdPollerBase::~FdPollerBase()
{
	delete[] entry_;
	delete timer_;
	delete pending_new_;
	delete retired_;
}

void FdPollerBase::reset_fd(int fd)
{
	PollerEntry* p;
	if (fd < 0 || fd >= (int)max_fds_) return;
	p = &entry_[fd];
	p->ev_mask = EV_NULL;
	p->status = FDS_EMPTY;
}


//init a new fd
int FdPollerBase::add_fd(int fd, IFdEventReactor *reactor)
{
	PollerEntry *pe;
	if (fd < 0 || fd >= (int)max_fds_) return AX_RET_ERROR;	
	pe = &entry_[fd];
	if (pe->status != FDS_EMPTY) {
		//somewhere call close(fd) instead of FdPollerBase::close_fd(),
		//some dirty data here, push it in pending_new_ and add_fd later.
		pending_new_->insert(fd, (void*)reactor);
		return AX_RET_AGAIN;
	}

	pe->reactor = reactor;
	pe->ev_mask = EV_NULL;
	pe->status = FDS_ACTIVE;
	//auto add ev_read by default
	add_event(fd, EV_READ);
	return AX_RET_OK;
}

void FdPollerBase::set_timetick(int tm)
{
	if (tm > 0) timetick_ = tm;
}

//set wait close state. the manager will close this fd later
//when fd is used as part of a key known by other process/thread, the fd should not 
//close immediately before notify all partner, in order to avoid be reuse by OS.
int FdPollerBase::wait_close(int fd)
{
	PollerEntry *pe;
	if (fd < 0 || fd >= (int)max_fds_) return AX_RET_ERROR;
	pe = &entry_[fd];
	if (pe->status != FDS_ACTIVE) return AX_RET_ERROR;
	del_event(fd, EV_READ|EV_WRITE);
	pe->status = FDS_WAIT_CLOSE;
	pe->reactor = NULL;
	return AX_RET_OK;
}

//add to retired fd. close it current loop completed
int FdPollerBase::close_fd(int fd)
{
	retired_[retired_sz_] = fd;
	++retired_sz_ ;
	return 0;
}

//change reactor
int FdPollerBase::set_reactor(int fd, IFdEventReactor *reactor)
{
	if (fd < 0 || fd >= (int)max_fds_) return AX_RET_ERROR;
	if (entry_[fd].status != FDS_ACTIVE) {
		return AX_RET_ERROR;
	}

	entry_[fd].reactor = reactor;
	return AX_RET_OK;
}

//return time pass from last call. (unit: ms)
uint32_t FdPollerBase::get_timepass()
{
	struct timeval tv;
	uint64_t tm_now;
	uint32_t pass;
	gettimeofday(&tv, NULL);
    tm_now = tv.tv_sec * 1000 + (tv.tv_usec / 1000);
	if (tm_last_ == 0) tm_last_ = tm_now;
	pass = tm_now - tm_last_;	
	tm_last_ = tm_now;
	return pass;
}

//do one poll. usually in a loop
int FdPollerBase::process()
{
	int i, fd;
	HashEntryInt *p;
	RT_ASSERT(timer_ != NULL);
	timer_->timer_routine();
	//all logic implement in do_poll()
	do_poll();

	//close retired fd
	for(i=0; i<retired_sz_; i++) {
		fd = retired_[i];
		del_event(fd, EV_READ|EV_WRITE);
		reset_fd(fd);
		if (entry_[fd].status != FDS_EMPTY) {
			::close(fd);
		}
	}
	retired_sz_ = 0;

	//handle conflict
	p = pending_new_->head();
	while (p != NULL) {
		fd = p->get_key();
		entry_[fd].reactor = (IFdEventReactor*)(p->data);
		entry_[fd].status = FDS_ACTIVE;
		entry_[fd].ev_mask = EV_NULL;
		add_event(fd, EV_READ);	
		p = p->get_link_next();
	}
	
	return 0;
}

int FdPollerBase::add_timer_event(ev_timer_proc cbfunc, void* data, uint32_t timeout)
{
	return timer_->add_timer(cbfunc, data, timeout);
}

int FdPollerBase::del_timer_event(int handle)
{
	return timer_->del_timer(handle);
}


} //namespace
