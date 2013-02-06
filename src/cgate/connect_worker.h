//thread worker 
#ifndef _AX_CONNECT_WORKER_H_
#define _AX_CONNECT_WORKER_H_

#include <core/ax_core.h>
#include <proto/node_msg_pb.h>
#include <proto/cgate_msg_pb.h>

using namespace xpb;
using namespace axon;

//hid store thread no, and auto increment to ensure unique
// | thread no 6bit | 26bit auto increment |

#define HID_THR_BITS  26
#define HID_THR_MASK 0x3F


class ConnectWorker: public IConnectHandler
{

public:
	enum {
		S_INVALID = 0,
		S_READY = 1,
		S_ACTIVE = 2,
		S_TO_CLOSE = 3,

	};

	static void thread_worker_fn(void * arg);
	static int on_mailbox_read(void* pobj, proto_msg_t* pb, int remain);
	void start_loop();
	int get_state() { return state_; }

	ConnectWorker(uint32_t max_connect, uint32_t rsize, uint32_t wsize);
	~ConnectWorker();

private:
	Connect* new_connect(int fd, int hid, uint64_t peer_ip, uint16_t peer_port);
	Connect* get_connect(int vfd, int hid);

	//client data processing
	void close_connect(int vfd, int hid);
	void wait_close(Connect*);
	virtual void on_read(Connect* conn);
	virtual void on_write(Connect* conn);
	void decrypt_client_data(Connect* conn);
	int process_data(Connect* conn);

	void send_worker_command(proto_msg_t * msg);
	int process_worker_command(pb_cgate_ctrl* msg, int remain);

private:
	EvPoller *poller_;
	ConnectContainer *container_;
	uint32_t rbuf_size_;
	uint32_t wbuf_size_;
	uint32_t max_connect_;
	int state_;
	tpipe_t *mailbox_;
};

#endif
