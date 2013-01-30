
#include "ax_debug.h"
#include "fd_poller.h"

namespace axon {

FdPollerBase::FdPollerBase(uint32_t maxn, int timetick) : timetick_(timetick_)
{
	fdmap_ = new HashMapInt(maxn);
	timer_ = new EvTimer(timetick_);
	max_fds_ = maxn;
	tm_last_ = 0;
}

FdPollerBase::~FdPollerBase()
{
	delete fdmap_;
}

FdPollerBase::PollerEntry* FdPollerBase::find_entry(int fd, uint32_t h)
{
	HashEntryInt *phash;
	PollerEntry *pe;

	phash = fdmap_->find(fd);
	if (phash == NULL) return NULL;
	pe = static_cast<PollerEntry*>(phash->data);
	if (pe->handle != h) return NULL;  //handle not equal
	return pe;
}

//return handle
ev_handle_t FdPollerBase::add_fd(int fd, IFdEventReactor *reactor)
{
	PollerEntry *entry;
	HashEntryInt *phash;

	phash = fdmap_->find(fd);
	if (phash != NULL) {
		entry = (PollerEntry*)(phash->data);
		if (entry != NULL && !(entry->ev_mask & EV_INVALID)) {
			//warning. replace original poller entry
			//Maybe there're some bugs in caller's code, or call add_fd several times
			del_event(fd, entry->handle, entry->ev_mask);
			DBG_WATCHV("add_fd several times. fd=%d, handle=%d", fd, entry->handle);
		}	
	} else {
		entry = new PollerEntry;
	}

	entry->handle = ++handle_;
	entry->fd = fd;
	entry->reactor = reactor;
	entry->ev_mask = 0;
	fdmap_->insert(fd, (void*)entry);	
	return entry->handle;
}

void FdPollerBase::set_timetick(int tm)
{
	if (tm > 0) timetick_ = tm;
}

int FdPollerBase::rm_fd(int fd)
{
	HashEntryInt *phash;
	PollerEntry *pe;

	phash = fdmap_->find(fd);
	if (NULL == phash) return -1;
	pe = (PollerEntry*)(phash->data);
	//set this fd to invalid. 
	//won't remove it  
	pe->ev_mask |= EV_INVALID;
	return 0;
}

int FdPollerBase::set_reactor(int fd, uint32_t h, IFdEventReactor *reactor)
{
	PollerEntry *pe;
	int mask;	
	pe = find_entry(fd, h);
	if (NULL == pe) return -1;
	mask = pe->ev_mask;
	del_event(fd, h, mask);
	pe->reactor = reactor;
	add_event(fd, h, mask);
	return 0;
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

int FdPollerBase::process()
{
	RT_ASSERT(timer_ != NULL);
	timer_->timer_routine();
	do_poll(timetick_);
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
