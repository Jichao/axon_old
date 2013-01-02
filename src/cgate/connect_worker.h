
#ifndef _AX_CONNECT_WORKER_H_
#define _AX_CONNECT_WORKER_H_

#include <core/ax_core.h>

using namespace axon;

class ConnectWorker: public IConnectHandler
{
public:
	ConnectWorker(EvPoller* poller);
	~ConnectWorker();
	Connect* new_connect(int fd, int hid);
	virtual void on_read(Connect* conn);
	virtual void on_write(Connect* conn);

private:
	EvPoller  *poller_;
	ConnectContainer *container_;
};

#endif
