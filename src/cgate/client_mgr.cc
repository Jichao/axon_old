
#include "connect_worker.h"
#include "client_mgr.h"

using namespace axon;

ClientMgr::ClientMgr() : inited_(0), client_listener_(NULL), worker_(NULL)
{
	hid_ = 1;   //start from 1
	client_port_ = 0;
}

ClientMgr::~ClientMgr()
{
	delete main_poller;
	delete client_listener;
}

ClientMgr::init(uint32_t max_connect, uint32_t rsize, uint32_t wsize, uint16_t client_port, uint16_t backlog)
{

	main_poller_ = new EvPoller(max_connect, 100);
	client_listener_ = new Listener;
	worker_ = new ConnectWorker(main_poller, max_connect, rize, wsize);
	client_listener->init(this, client_port, backlog);

	inited_ = 1;
}

void ClientMgr::on_listen_read(Listener *ls)
{
	socklen_t length;
	struct sockaddr_in client_addr;
	int client_fd;

	length = sizeof(struct sockaddr_in);
	client_fd = ::accept(ls->fd_, (struct sockaddr *)&client_addr, (socklen_t *)
&length);
	//set nonblocking here
	ax_set_nonblock(client_fd);

	accept_new_connect(client_fd);	
}

int ClientMgr::accept_new_connect(int sockfd)
{
	return 1;
}
