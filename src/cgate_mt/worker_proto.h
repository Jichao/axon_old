
#ifndef _WORKER_PROTO_H_
#define _WORKER_PROTO_H_

#include <core/ax_core.h>

using namespace axon;

//worker thread init info(as a thread create param)
typedef struct worker_init_s
{
	int max_conn;
	tpipe_t *mailbox;
} worker_init_t;


//protocol=============================================

//var_msg type
#define T_MW_CMD 1
#define T_WM_CMD 2
#define T_NEWCONN 3
#define T_CLI_UNICAST 4    //unicast to a client
#define T_CLI_MULTICAST 5  //multicast to a bunch of clients
#define T_CLI_RECV 6       //receive from client

//Main->Worker  new connection
struct newconn_msg_t
{
	int fd;
	uint32_t hid;
	unsigned long ip;
	uint16_t port;
};


//main thread ==> worker message
class mw_msg_t
{
public:
	mw_msg_t() {}
	wm_msg_t(int c)  { cmd = c;}

	enum {

	};
	int cmd;
	union {
		int val;
	} u;

};

//worker thread ==> main thread
class wm_msg_t
{
public:
	wm_msg_t() {}
	wm_msg_t(int c)  { cmd = c; }

	enum {
		INIT_OK = 1,   //worker init ok

	};

	int cmd;
	union {
		int val; 
	} u;

};


#endif
