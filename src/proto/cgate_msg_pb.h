//============================================
//proto_msg definition
//Warning:This file is auto generated. Don't modify it by hand
//============================================

#ifndef _cgate_msg_PB_
#define _cgate_msg_PB_

#include <core/ax_proto_msg.h>
using namespace axon;

namespace xpb {
//proto package ID
const int PKG_CGATE_MSG = 1;

//proto id name define

const int PT_CGATE_NEWCONN = 1;
const int PT_WORKER_CTRL = 2;
const int PT_WORKER_NOTIFY = 2;


class pb_cgate_newconn : public proto_msg_t
{
public:
	static const int proto_id = 1;
	pb_cgate_newconn () {
		fd = 0;
		hid = 0;
		ip = 0;
		port = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int32_t fd;
	uint32_t hid;
	uint64_t ip;
	uint16_t port;
	
};


class pb_worker_ctrl : public proto_msg_t
{
public:
	static const int proto_id = 2;
	pb_worker_ctrl () {
		cmd = 0;
		lparam = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int16_t cmd;
	int32_t lparam;
	
};


class pb_worker_notify : public proto_msg_t
{
public:
	static const int proto_id = 2;
	pb_worker_notify () {
		cmd = 0;
		lparam = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int16_t cmd;
	int32_t lparam;
	
};



} //namespace xpb
#endif
