//manager a bundle of connection which share the same poller
//it may be run in main thread or a separate thread(different poller)

#include "connect_worker.h"
#include "node_init.h"

using namespace axon;
using namespace xpb;

ConnectWorker::ConnectWorker(uint32_t max_connect, uint32_t rsize, uint32_t wsize)
{
	poller_ = new EvPoller(max_connect, NodeConf::timetick);
	rbuf_size_ = rsize;
	wbuf_size_ = wsize;
	max_connect_ = max_connect;
	container_ = new ConnectContainer(max_connect, this, rsize, wsize);
	mailbox_ = NULL;
	state_ = S_INVALID;
	idx_ = -1;
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
	pworker->idx_ = pinit->idx;
	pworker->mailbox_->init(1, pworker->poller_, on_mailbox_read, pworker);
	pworker->start_loop();
}


Connect* ConnectWorker::get_connect(int vfd, int hid)
{
	Connect* c = container_->get_connect(vfd);
	if (NULL == c) return NULL;
	if (c->hid_ != hid) return NULL;
	return c;
}

//notity other before close
void ConnectWorker::wait_close(Connect* c)
{
	pb_conn_notify msg;
   	RT_ASSERT(c);
	poller_->wait_close(c->fd_);
	//notify other node and wait close
	msg.fd = c->fd_;
	msg.hid = c->hid_;	
	msg.action = pb_conn_notify::PEER_CLOSE;
	send_worker_command(PT_CONN_NOTIFY, &msg);
}


//force close vfd
void ConnectWorker::close_connect(int vfd, int hid)
{
	Connect* c;

    c = container_->get_connect(vfd);
	if (NULL == c) return;
	if (c->hid_ != hid) {
		//connection conflict !!!
		return;
	}
	poller_->close_fd(c->fd_);
	container_->free_connect(vfd);
}

//new connection
Connect* ConnectWorker::new_connect(int fd, int hid, uint64_t peer_ip, uint16_t peer_port)
{
	int vfd = container_->alloc_connect(fd);
	int ret;
	Connect* conn;
	if (vfd < 0) {
		return NULL;  //no more connection to alloc
	}	
	conn = container_->get_connect(vfd);
	RT_ASSERT(conn != NULL);
	ret = conn->init(this, fd, rbuf_size_, wbuf_size_);
	if (ret < 0) {
		debug_log("[ERROR] ConnectWorker cannot create new_connect. fd=%d,vfd=%d, hid=%d", fd, vfd, hid);
		//error
		return NULL;
	}
	conn->index_ = vfd;
	conn->hid_ = hid;
	conn->peer_port_ = peer_port;
	conn->peer_ip_ = peer_ip;
	conn->set_status(CONN_ACTIVE);
	poller_->add_fd(fd, conn);
	return conn;
}

//client data in
void ConnectWorker::on_read(Connect* conn)
{
	int ret;
	ret = conn->read();	
	if (ret < 0 && (errno == EINTR || errno == EAGAIN)) {
		return;   //no data, try next time
	}
	if (ret == 0) return;
	if (ret < 0) {
		//client socket fail/close
		wait_close(conn);
		//temp test
		close_connect(conn->index_, conn->hid_);
		return;
	}
	process_data(conn);
}

//client data out
void ConnectWorker::on_write(Connect* conn)
{

}


int ConnectWorker::process_data(Connect* conn)
{
	int len;
	int ret;
	//echo back
	len = conn->rbuf_->len();	
	DBG_WATCHV("ConnectWorker::process_data. fd:%d len:%d", conn->fd_, len);
	if (len > 0) {
		ret = ::write(conn->fd_, conn->rbuf_->data(), len);
	}
	conn->rbuf_->pop(len);
	return 0;
}


//message from main thread
void ConnectWorker::on_mailbox_read(void* pobj, msg_header_t header, char* p)
{
	int ret;
	int remain;
	int wrapper_type;
	ConnectWorker *pmgr = (ConnectWorker*)pobj;

	remain = header.pl_len;
	wrapper_type = header.proto;
	if (wrapper_type == PT_CGATE_CTRL) {
		pb_cgate_ctrl ctrl;
		ret = ctrl.unpack(p, remain);
		if (ret < 0) return;
		p += ret;
		remain -= ret;
		if (ctrl.cmd == PT_CGATE_NEWCONN) {
			//new client connection
			pb_cgate_newconn pkt_conn;
			ret = pkt_conn.unpack(p, remain);
			if (ret < 0) return;
			pmgr->new_connect(pkt_conn.fd, pkt_conn.hid, pkt_conn.ip, pkt_conn.port);
			
		} else if (ctrl.cmd == PT_CONN_CTRL) {
			pmgr->process_conn_ctrl(p + ret, remain);
		} else {
			pmgr->process_worker_command(ctrl.cmd, p + ret, remain);
		}
	}
}

void ConnectWorker::process_conn_ctrl(char *p, int remain)
{
	int ret, hid, vfd;
	pb_conn_ctrl pkt;
	ret = pkt.unpack(p, remain);
	if (ret < 0) return;
}

//process command from main thread
void ConnectWorker::process_worker_command(int proto, char *p, int remain)
{
	int ret = 0;

}

//send control command to main thread
void ConnectWorker::send_worker_command(int cmd, proto_msg_t *msg)
{
	pb_cgate_ctrl pkt_ctrl;
	pkt_ctrl.cmd = cmd;
	mailbox_->write(1, PT_CGATE_CTRL, &pkt_ctrl, msg);
}

void ConnectWorker::start_loop()
{
	pb_worker_notify pkt_notify;
	//notify main thread ready
	state_ = S_ACTIVE;

	pkt_notify.cmd = pb_worker_notify::WORKER_START_OK;
	pkt_notify.lparam = idx_;
	send_worker_command(PT_WORKER_NOTIFY, &pkt_notify);

	while(state_ != ConnectWorker::S_TO_CLOSE) {
		poller_->process();
	}
}

