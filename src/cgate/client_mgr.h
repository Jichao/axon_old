
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
	int accept_new_connect(int sockfd);

	EvPoller *poller;
	Listener *client_listener;
	ConnectWorker *conn_worker;
private:
	int hid_;   //alloc hid to a new connection
};



#endif
