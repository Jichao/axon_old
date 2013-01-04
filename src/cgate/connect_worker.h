
#ifndef _AX_CONNECT_WORKER_H_
#define _AX_CONNECT_WORKER_H_

#include <core/ax_core.h>

using namespace axon;

class ConnectWorker: public IConnectHandler
{
public:
	ConnectWorker(EvPoller* poller, uint32_t max_connect, uint32_t rsize, uint32_t wsize);
	~ConnectWorker();
	Connect* new_connect(int fd, int hid, uint16_t peer_port, string_t peer_ip);
	virtual void on_read(Connect* conn);
	virtual void on_write(Connect* conn);

private:
	EvPoller  *poller_;
	ConnectContainer *container_;
	uint32_t rbuf_size_;
	uint32_t wbuf_size_;
	uint32_t max_connect_;
};

#endif
