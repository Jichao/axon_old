//manager a bundle of connection which share the same poller
//it may be run in main thread or a separate thread(different poller)

#include "connect_worker.h"

ConnectWorker::ConnectWorker(EvPoller* poller, uint32_t max_connect, uint32_t rsize, uint32_t wsize)
{
	poller_ = poller;
	rbuf_size_ = rsize;
	wbuf_size_ = wsize;
	max_connect_ = max_connect;
	container_ = new ConnectConainter(max_connect, this, rsize, wsize);
}

ConnectWorker::~ConnectWorker()
{
	poller_ = NULL;
	delete container_;
}

Connect* ConnectWorker::new_connect(int fd, int hid, uint16_t peer_port, string_t peer_ip)
{
	int vfd = container_->alloc_connect(fd);
	Connect* conn;
	if (vfd < 0) {
		return NULL;  //no more connection to alloc
	}	
	conn = container_->get_connect();
	RT_ASSERT(conn != NULL);
	conn->init(this, fd, rbuf_size_, wbuf_size_);
	conn->hid_ = hid;
	conn->peer_port_ = peer_port;
	conn->peer_ip_ = peer_ip;
	conn->ev_handle_ = poller_->add_fd(fd, conn);
	poller_->add_event(fd, hid, EV_READ);
}

void ConnectWorker::on_read(Connect* conn)
{

}

void ConnectWorker::on_write(Connect* conn)
{

}

