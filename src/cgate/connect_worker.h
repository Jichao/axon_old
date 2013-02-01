//thread worker 
#ifndef _AX_CONNECT_WORKER_H_
#define _AX_CONNECT_WORKER_H_

#include <core/ax_core.h>

using namespace axon;

//hid store thread no, and auto increment to ensure unique
// | thread no 6bit | 26bit auto increment |

#define HID_THR_BITS  26
#define HID_THR_MASK 0x3F


class ConnectWorker: public IConnectHandler
{
public:
	static void thread_worker_fn(void * arg);
	static void on_mailbox_read(void* pobj, var_msg_t* data);

	ConnectWorker(uint32_t max_connect, uint32_t rsize, uint32_t wsize);
	~ConnectWorker();

private:
	Connect* new_connect(int fd, int hid, uint32_t peer_ip, uint16_t peer_port);
	Connect* get_connect(int vfd, int hid);
	void close_connect(int vfd, int hid);
	void wait_close(Connect*);
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
