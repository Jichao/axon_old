//thread worker 
#ifndef _AX_CONNECT_WORKER_H_
#define _AX_CONNECT_WORKER_H_

#include <core/ax_core.h>

using namespace axon;

class ConnectWorker: public IConnectHandler
{
public:
	static void thread_worker_fn(void * arg);
	static void on_mailbox_read(void* pobj, var_msg_t* data);

	ConnectWorker(uint32_t max_connect, uint32_t rsize, uint32_t wsize);
	~ConnectWorker();
	Connect* new_connect(int fd, int hid, string_t peer_ip, uint16_t peer_port);
	Connect* get_connect(int vfd, int hid);
	void close_connect(int vfd, int hid);
	virtual void on_read(Connect* conn);
	virtual void on_write(Connect* conn);
	void decrypt_client_data(Connect* conn);
	void process_data(Connect* conn);

private:
	EvPoller *poller_;
	ConnectContainer *container_;
	uint32_t rbuf_size_;
	uint32_t wbuf_size_;
	uint32_t max_connect_;
	tpipe_t *mailbox_;
};

#endif
