//epoll event driver for Linux

#if defined USE_EPOLL

#include "epoll.h"
#include <sys/epoll.h>

namespace axon {

EpollPoller::EpollPoller(uint32_t maxn, int timetick) : FdPollerBase(maxn, timetick)
{
	epoll_fd_ = epoll_create(100);
	event_list_ = (struct epoll_event*)malloc(sizeof(struct epoll_event) * max_fds_);
}

EpollPoller::~EpollPoller()
{
	free(event_list_);
	event_list_ = NULL;
	epoll_fd_ = 0;
}

int EpollPoller::add_event(int fd, uint32_t h, int flag)
{
	struct epoll_event wep;
	struct epoll_event rep;
	PollerEntry *pe = find_entry(fd, h);
	if (pe == NULL) return AX_RET_ERROR;

	if (flag & EV_READ) {
		rep.events = EPOLLIN;
		rep.data.ptr = (void*)pe;
		rep.data.fd = fd;
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &rep) == -1) {
			return AX_RET_ERROR;
		}
		pe->ev_mask |= EV_READ;
	}
	if (flag & EV_WRITE) {
		wep.events = EPOLLOUT;
		wep.data.ptr = (void*)pe;
		wep.data.fd = fd;
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &wep) == -1) {
			return AX_RET_ERROR;
		}

		pe->ev_mask |= EV_WRITE;
	}
	return AX_RET_OK;
}

int EpollPoller::del_event(int fd, uint32_t h, int flag)
{
	PollerEntry *pe;
	struct epoll_event rep;
	struct epoll_event wep;
	pe = find_entry(fd, h);
	if (NULL == pe) return AX_RET_ERROR;
	if (flag & EV_READ) {
		rep.events = EPOLLIN;
		rep.data.fd = fd;
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &rep) == -1) {
			return AX_RET_ERROR;
		}
		pe->ev_mask &= ~EV_READ;
	}
	if (flag & EV_WRITE) {
		wep.events = EPOLLOUT;
		wep.data.fd = fd;
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &wep) == -1) {
			return AX_RET_ERROR;
		}

		pe->ev_mask &= ~EV_WRITE;
	}
	return AX_RET_OK;
}

int EpollPoller::do_poll(int timeout)
{
	int events;
	int fd;
	struct epoll_event *ep;
	PollerEntry *pe, *pmapfd;

	if (timeout > 0) {
		events = epoll_wait(epoll_fd_, event_list_, max_fds_, timeout);
	} else {
		events = epoll_wait(epoll_fd_, event_list_, max_fds_, -1);
	}

	if (events < 0) return AX_RET_ERROR;
	for (int i=0; i<events; i++) {
		ep = &event_list_[i];
		fd = ep->data.fd;
		pe = (PollerEntry*)ep->data.ptr;
		pmapfd = (PollerEntry*)fdmap_->get_data(fd);
		if (pmapfd == NULL) continue;
		if (pmapfd->handle != pe->handle) continue;
		if (pmapfd->ev_mask & EV_INVALID) {
			del_event(fd, pe->handle, EV_READ | EV_WRITE);
			continue;
		}
		if (ep->events & EPOLLIN) {
			pe->reactor->on_ev_read(fd);
		} 
		if (ep->events & EPOLLOUT) {
			pe->reactor->on_ev_write(fd);
		}
	}
	return events;
}

} //namespace

#endif
