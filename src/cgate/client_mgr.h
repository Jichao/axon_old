
#ifndef _CLIENT_MGR_H_
#define _CLIENT_MGR_H_

#include <core/ax_core.h>
#include <proto/node_msg_pb.h>
#include <proto/cgate_msg_pb.h>
#include "client_mgr.h"
#include "connect_worker.h"

using namespace axon;
using namespace xpb;

struct worker_init_t
{
	uint32_t max_conn;
	tpipe_t *mailbox;
};

class ClientMgr: IListenHandler
{
public:
	ClientMgr(EvPoller *poller);
	~ClientMgr();
	virtual void on_listen_read(Listener* ls);
	void init(uint16_t client_port, uint16_t backlog, int worker_num);
	int start_worker();  //start worker's thread

	int listen();
	int notify_workers(proto_msg_t* msg);
	int process();

	EvPoller* get_poller();
	void on_peer_close(int fd, int hid);
	int close_connect(int hid);
	static int on_mailbox_read(void* pobj, proto_msg_t *pb, int remain);

private:
	int send_worker_ctrl(pb_cgate_ctrl * msg);
	int on_accept_new_connect(int sockfd, uint64_t ip, uint16_t port);
	int process_worker_command(pb_cgate_ctrl *wrapper, int remain);

private:
	//be careful: some var shared between main thread and worker thread 
	typedef struct thread_worker_s {
		AxThread thr;     //worker's thread
		int active_conn;  //active connection
		int max_conn;     //connection capacity
		uint8_t status;
		tpipe_t mailbox;  //queue between main thread and worker
		worker_init_t info;  //init info
	}thread_worker_t;

	int worker_num_;
	int inited_;  //initilized flag
	uint32_t hid_;   //alloc hid to a new connection
	thread_worker_t *workers_;	

	ConnectWorker *__worker_;   //connection routine worker 
	EvPoller *main_poller_;   //main poller
	Listener *client_listener_;
	HashMapInt *active_hids_;   //available hid table

};



#endif
