
#include "connect_worker.h"
#include "client_mgr.h"

using namespace axon;

ClientMgr::ClientMgr()
{
	client_listener = new Listener;
}

ClientMgr::~ClientMgr()
{

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
