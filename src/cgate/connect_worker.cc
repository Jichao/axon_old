//manager a bundle of connection which share the same poller
//it may be run in main thread or a separate thread(different poller)

#include "connect_worker.h"
#include "node_init.h"

ConnectWorker::ConnectWorker(uint32_t max_connect, uint32_t rsize, uint32_t wsize)
{
	poller_ = new EvPoller(max_connect, NodeConf::timetick);
	rbuf_size_ = rsize;
	wbuf_size_ = wsize;
	max_connect_ = max_connect;
	container_ = new ConnectContainer(max_connect, this, rsize, wsize);
	mailbox_ = NULL;
}

ConnectWorker::~ConnectWorker()
{
	poller_ = NULL;
	delete container_;
}

//thread function of connect worker
void ConnectWorker::thread_worker_fn(void * arg)
{
	worker_init_t *pinit = (worker_init_t*)arg;
	ConnectWorker *pworker = new ConnectWorker(pinit->max_conn, NodeConf::client_rbuf_size, 256);
	pworker->mailbox_ = pinit->mailbox;
	pworker->mailbox_->init(1, pworker->poller_, on_mailbox_read, pworker);
	//loop
}

Connect* ConnectWorker::get_connect(int vfd, int hid)
{
	Connect* c = container_->get_connect(vfd);
	if (NULL == c) return NULL;
	if (c->hid_ != hid) return NULL;
	return c;
}

void ConnectWorker::close_connect(int vfd, int hid)
{
	Connect* c = container_->get_connect(vfd);
	if (NULL == c) return;
	c->close();
	container_->free_connect(vfd);
}


Connect* ConnectWorker::new_connect(int fd, int hid, string_t peer_ip, uint16_t peer_port)
{
	int vfd = container_->alloc_connect(fd);
	Connect* conn;
	if (vfd < 0) {
		return NULL;  //no more connection to alloc
	}	
	conn = container_->get_connect(vfd);
	RT_ASSERT(conn != NULL);
	conn->init(this, fd, rbuf_size_, wbuf_size_);
	conn->hid_ = hid;
	conn->index_ = vfd;
	conn->peer_port_ = peer_port;
	conn->peer_ip_ = peer_ip;
	conn->ev_handle_ = poller_->add_fd(fd, conn);
	poller_->add_event(fd, conn->ev_handle_, EV_READ);
	return conn;
}

//data in
void ConnectWorker::on_read(Connect* conn)
{
	int ret;
	ret = conn->read();	
	if (ret < 0 && (errno == EINTR || errno == EAGAIN)) {
		return;   //no data, try next time
	}
	if (ret <= 0) {
		//client socket fail/close
		this->close_connect(conn->index_, conn->hid_);
		return;
	}
	process_data(conn);
}

//data out
void ConnectWorker::on_write(Connect* conn)
{

}

//decrypt stream
void ConnectWorker::decrypt_client_data(Connect* conn)
{


}

void ConnectWorker::process_data(Connect* conn)
{

	decrypt_client_data(conn);

}


//message from main thread
void ConnectWorker::on_mailbox_read(void* pobj, var_msg_t* data)
{


}
