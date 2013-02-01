
#ifndef _WORKER_PROTO_H_
#define _WORKER_PROTO_H_

#include <core/ax_core.h>

using namespace axon;

//worker thread init info
typedef struct worker_init_s
{
	int max_conn;
	tpipe_t *mailbox;
} worker_init_t;


#define CW_CMD 1
#define CM_CMD 2
#define CW_NEWCONN 3

struct newconn_msg_t
{
	int fd;
	uint32_t hid;
	uint32_t ip;
	uint16_t port;
};



//main thread ==> worker message
class cw_msg_t
{
public:
	int cmd;
	union {
		int dummy;
	} u;

};

//worker thread ==> main thread
class cm_msg_t
{
public:
	enum {
		CM_INIT_OK = 1,

	};

	int cmd;
	union {
		int dummy;
	} u;

};


#endif
