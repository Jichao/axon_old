//kqueue for FreeBsd

#if defined USE_KQUEUE

#include "kqueue.h"
#include <sys/event.h>
#include "common_def.h"

namespace axon {

KqueuePoller::KqueuePoller(uint32_t maxn, int timetick) : FdPollerBase(maxn, timetick)
{
	kqueue_fd_ = kqueue();
	event_list_ = (struct kevent*)malloc(sizeof(struct kevent) * max_fds_);
}

KqueuePoller::~KqueuePoller()
{
	free(event_list_);
}

int KqueuePoller::add_event(int fd, int flag)
{
	struct kevent ev;
	PollerEntry *pe;
	if (fd < 0 || fd > (int)max_fds_) return AX_RET_ERROR;
    pe = &entry_[fd];

	if (flag & EV_READ) {
		EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask |= EV_READ;
	}
	if (flag & EV_WRITE) {
		EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask |= EV_WRITE;
	}
	return AX_RET_OK;
}

int KqueuePoller::del_event(int fd, int flag)
{
	PollerEntry *pe;
	struct kevent ev;
	if (fd < 0 || fd > (int)max_fds_) return AX_RET_ERROR;
	pe = &entry_[fd];

	if (flag & EV_READ) {
		EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask &= ~EV_READ;
	}
	if (flag & EV_WRITE) {
		EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask &= ~EV_WRITE;
	}
	return 0;
}

//return: event count
//		  -1 error occur
int KqueuePoller::do_poll()
{
	int events;
	int fd;
	struct timespec ts;
	struct timespec *tp = NULL;
	struct kevent *kev;
	PollerEntry *pe;

	ts.tv_sec = timetick_ / 1000;
	ts.tv_nsec = (timetick_ % 1000) * 1000000;
	tp = &ts;

	events = kevent(kqueue_fd_, 0, 0, event_list_, max_fds_, tp);

	if (events < 0) return AX_RET_ERROR;
	for (int i=0; i<events; i++) {
		kev = &event_list_[i];
		fd = kev->ident;
		pe = &entry_[fd];
		if (pe->status != FDS_ACTIVE) {
			//will be close later
			del_event(fd, EV_READ | EV_WRITE);
			continue;
		}
		if (kev->filter == EVFILT_READ) {
			pe->reactor->on_ev_read(fd);
		} else if (kev->filter == EVFILT_WRITE) {
			pe->reactor->on_ev_write(fd);
		}
	}
	return events;
}

} //namespace

#endif
