//kqueue for FreeBsd

#if defined USE_KQUEUE

#include "kqueue.h"
#include <sys/event.h>

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

int KqueuePoller::add_event(int fd, uint32_t h, int flag)
{
	struct kevent ev;
	PollerEntry *pe = find_entry(fd, h);
	if (pe == NULL) return -1;

	if (flag & EV_READ) {
		EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, (void*)pe);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask |= EV_READ;
	}
	if (flag & EV_WRITE) {
		EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, (void*)pe);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask |= EV_WRITE;
	}
	return 0;
}

int KqueuePoller::del_event(int fd, uint32_t h, int flag)
{
	PollerEntry *pe;
	struct kevent ev;
	pe = find_entry(fd, h);
	if (NULL == pe) return -1;
	if (flag & EV_READ) {
		EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)pe);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask &= ~EV_READ;
	}
	if (flag & EV_WRITE) {
		EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)pe);
		kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
		pe->ev_mask &= ~EV_WRITE;
	}
	return 0;
}

int KqueuePoller::do_poll(int timeout)
{
	int events;
	int fd;
	struct timespec ts;
	struct timespec *tp = NULL;
	struct kevent *kev;
	PollerEntry *pe, *pmapfd;

	if (timeout > 0) {
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000000;
		tp = &ts;
	}
	events = kevent(kqueue_fd_, 0, 0, event_list_, max_fds_, tp);

	if (events < 0) return -1;
	for (int i=0; i<events; i++) {
		kev = &event_list_[i];
		fd = kev->ident;
		pe = (PollerEntry*)kev->udata;
		pmapfd = (PollerEntry*)fdmap_->get_data(fd);
		if (pmapfd == NULL) continue;
		if (pmapfd->handle != pe->handle) continue;
		if (pmapfd->ev_mask & EV_INVALID) {
			del_event(fd, pe->handle, EV_READ | EV_WRITE);
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
