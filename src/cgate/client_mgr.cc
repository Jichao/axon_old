
#include "node_init.h"
#include "connect_worker.h"
#include "client_mgr.h"

using namespace axon;

ClientMgr::ClientMgr() : inited_(0), client_listener_(NULL)
{
	worker_ = NULL;
	hid_ = 1;   //start from 1
	active_hids_ = NULL;
}

ClientMgr::~ClientMgr()
{
	delete worker_;
	delete main_poller_;
	delete client_listener_;
	delete active_hids_;
}

void ClientMgr::init(uint32_t max_connect, uint32_t rsize, uint32_t wsize, uint16_t client_port, uint16_t backlog)
{
	
	active_hids_ = new HashMapInt(max_connect * 2 / 3);
	main_poller_ = new EvPoller(max_connect, NodeConf::timetick);
	client_listener_ = new Listener;
	worker_ = new ConnectWorker(main_poller_, max_connect, rsize, wsize);
	client_listener_->init(this, client_port, backlog);

	inited_ = 1;
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
	main_poller_->process();
	return AX_RET_OK;
}


void ClientMgr::on_listen_read(Listener *ls)
{
	socklen_t length;
	struct sockaddr_in client_addr;
	int client_fd;
	uint16_t client_port;
	string_t client_ip;
	int accept_once = 0;

	length = sizeof(struct sockaddr_in);
	while (accept_once < 100) {
		client_fd = ::accept(ls->fd_, (struct sockaddr *)&client_addr, (socklen_t *)&length);
		if (client_fd < 0) {
			if (errno == EWOULDBLOCK) continue;
			//error occur
			break;
		}
		client_ip.strcat( inet_ntoa(client_addr.sin_addr) );
		client_port = ntohs(client_addr.sin_port);
		on_accept_new_connect(client_fd, client_ip, client_port);	
		++accept_once;
	}
}

//accept new socket and dispatch to a worker
int ClientMgr::on_accept_new_connect(int sockfd, string_t ip, uint16_t port)
{
	Connect* conn;
	//accept
	RT_ASSERT(inited_ != 0);
	
	//set nonblocking here
	ax_set_nonblock(sockfd);
	++hid_;
	conn = worker_->new_connect(sockfd, hid_, ip, port);
	if (NULL == conn) {
		return AX_RET_ERROR;
	}
	active_hids_->insert(hid_, (void*)conn->index_);
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
	Connect *conn;
	vfd = (long)(active_hids_->remove_get(hid));
}

//active close connect in server-side
int ClientMgr::close_connect(int hid)
{
	int vfd;
	Connect* conn;

	vfd = (long)(active_hids_->remove_get(hid));
	conn = worker_->get_connect(vfd, hid);
	if (NULL == conn) return AX_RET_ERROR;
	return AX_RET_OK;
}
