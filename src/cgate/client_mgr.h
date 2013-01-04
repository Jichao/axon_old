
#ifndef _CLIENT_MGR_H_
#define _CLIENT_MGR_H_

#include <core/ax_core.h>
#include "client_mgr.h"

using namespace axon;

class ClientMgr: IListenHandler
{
public:
	ClientMgr();
	~ClientMgr();
	virtual void on_listen_read(Listener* ls);
	void init(uint32_t max_connect, uint32_t rsize, uint32_t wsize, uint16_t client_port, uint16_t backlog);
	EvPoller* get_poller();
	void on_peer_close(int fd, int hid);
	int close_connect(int hid);

private:
	int on_accept_new_connect(int sockfd, string_t ip, uint16_t port);

private:
	ConnectWorker *worker_;   //connection routine worker 
	EvPoller *main_poller_;   //main poller
	Listener *client_listener_;
	HashMapInt *active_hids_;   //available hid table

	int hid_;   //alloc hid to a new connection
	int inited_;  //initilized flag
};



#endif
