
#include "node_init.h"
#include "connect_worker.h"
#include "client_mgr.h"
#include "worker_proto.h"
#include "node_def.h"

using namespace axon;

ClientMgr::ClientMgr(EvPoller *poller) : inited_(0), client_listener_(NULL)
{
	hid_ = 1;   //start from 1
	active_hids_ = NULL;
	main_poller_ = poller;
	workers_ = NULL;
}

ClientMgr::~ClientMgr()
{
	delete client_listener_;
	delete active_hids_;
	if (workers_) {
		for (int i=0; i<worker_num_; i++) {
			if (workers_[i].status > 0) {
				workers_[i].thr.stop();
			}
		}
		delete[] workers_;
	}
}

void ClientMgr::init(uint16_t client_port, uint16_t backlog, int worker_num)
{
	
	active_hids_ = new HashMapInt(NodeConf::max_connect * 2 / 3);
	client_listener_ = new Listener;
	client_listener_->init(this, client_port, backlog);
	RT_ASSERT(worker_num > 0);
	if (worker_num > 32) {
		//too much thread
		worker_num = 32;
	}
	worker_num_ = worker_num;
	workers_ = new thread_worker_t[worker_num_];

	inited_ = 1;

}

//broadcast message to worker threads
int ClientMgr::notify_workers(cw_msg_t* msg)
{
	for(int i=0; i<worker_num_; i++) {
		if (workers_[i].status < 1) continue;
		workers_[i].mailbox.write(0, sizeof(cw_msg_t), CW_CMD, (char*)msg); 
	}
	return 0;
}

//start connect worker
int ClientMgr::start_worker()
{
	int i, ret;

	//init worker first
	if (!inited_) return AX_RET_ERROR;
	for(i=0; i<worker_num_; i++) {
		//init mailbox	
		workers_[i].mailbox.init(0, main_poller_, on_mailbox_read, this);
		workers_[i].status = 0;
		workers_[i].active_conn = 0;
		workers_[i].max_conn = NodeConf::max_connect / worker_num_ + 100;
		workers_[i].info.max_conn = workers_[i].max_conn;
		workers_[i].info.mailbox = &(workers_[i].mailbox);

		ret = workers_[i].thr.start(ConnectWorker::thread_worker_fn, (void*)(&(workers_[i].info)));
		if (ret == 0) {
			workers_[i].status = 1;
		}
	}
	return 0;
}

//static callback function
void ClientMgr::on_mailbox_read(void *pobj, var_msg_t *data)
{
	ClientMgr *mgr = (ClientMgr*)pobj;
}

int ClientMgr::listen()
{
	int ret;
	if (!inited_) return AX_RET_ERROR;
	ret = client_listener_->listen(main_poller_);
	if (ret == AX_RET_ERROR) {
		debug_log("[Listener] cannot bind listener. port=%d", client_listener_->port_);
		return AX_RET_ERROR;
	} 
	debug_log("[Listener] start listen on client port=%d", client_listener_->get_port());
	return AX_RET_OK;
}

int ClientMgr::process()
{
	while( g_node_state == NODE_RUNNING) {
		main_poller_->process();
	}
	for(int i=0; i<worker_num_; i++) {
		if (workers_[i].status == 1) {
			workers_[i].thr.stop();
		}
	}

	return AX_RET_OK;
}


void ClientMgr::on_listen_read(Listener *ls)
{
	socklen_t length;
	struct sockaddr_in client_addr;
	int client_fd;
	uint16_t client_port;
	int accept_once = 0;

	length = sizeof(struct sockaddr_in);
	while (accept_once < 100) {
		client_fd = ::accept(ls->fd_, (struct sockaddr *)&client_addr, (socklen_t *)&length);
		if (client_fd < 0) {
			if (errno == EWOULDBLOCK || errno == EINTR) { 
				break;	
			}
			//error occur
			break;

		}
		client_port = ntohs(client_addr.sin_port);
		on_accept_new_connect(client_fd, htonl(client_addr.sin_addr.s_addr), client_port);	
		++accept_once;
	}
}

//accept new socket and dispatch to a worker
int ClientMgr::on_accept_new_connect(int sockfd, uint32_t ip, uint16_t port)
{
	int load, select = 0;
	tpipe_t *mailbox;
	newconn_msg_t msg;
	//accept
	RT_ASSERT(inited_ != 0);
	
	//set nonblocking here
	ax_set_nonblock(sockfd);
	++hid_;
	if (hid_ >= ~(HID_THR_MASK <<HID_THR_BITS) ) {
		hid_ = 1;
	}
	load = workers_[0].active_conn;
	//find least burden worker
	for(int i=1; i<worker_num_; i++) {
		if (load > workers_[1].active_conn) {
			load = workers_[1].active_conn;
			select = i;
		}	
	}

	mailbox = &(workers_[select].mailbox);
	msg.fd = sockfd;
	msg.hid = (select << HID_THR_BITS) | hid_;
	msg.ip = ip;
	msg.port = port;

	mailbox->write(0, sizeof(msg), CW_NEWCONN, (char*)&msg);

	return AX_RET_OK;
}

EvPoller* ClientMgr::get_poller()
{
	return main_poller_;
}

//client close actively
void ClientMgr::on_peer_close(int fd, int hid)
{
	int vfd;
	vfd = (long)(active_hids_->remove_get(hid));
}

//active close connect in server-side
int ClientMgr::close_connect(int hid)
{
	int vfd;
	vfd = (hid >> HID_THR_BITS) & HID_THR_MASK;
	return AX_RET_OK;
}
