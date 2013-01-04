
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
	main_poller_ = new EvPoller(max_connect, 100);
	client_listener_ = new Listener;
	worker_ = new ConnectWorker(main_poller_, max_connect, rsize, wsize);
	client_listener_->init(this, client_port, backlog);

	inited_ = 1;
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
			break;  //error occur
		}
		client_ip.strcat( inet_ntoa(client_addr.sin_addr) );
		client_port = ntohs(client_addr.sin_port);
		accept_new_connect(client_fd, client_ip, client_port);	
		++accept_once;
	}
}

//accept new socket and dispatch to a worker
int ClientMgr::accept_new_connect(int sockfd, string_t ip, uint16_t port)
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
