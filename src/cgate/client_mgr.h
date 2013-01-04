
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
	int accept_new_connect(int sockfd);
	int get_poller();

private:

	ConnectWorker *worker_;   //connection routine worker 
	EvPoller *main_poller;   //main poller
	Listener *client_listener_;
	int hid_;   //alloc hid to a new connection
	int inited_;  //initilized flag
};



#endif
