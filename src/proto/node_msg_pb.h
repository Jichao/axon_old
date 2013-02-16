//============================================
//proto_msg definition
//Warning:This file is auto generated. Don't modify it by hand
//============================================

#ifndef _node_msg_PB_
#define _node_msg_PB_

#include <core/ax_proto_msg.h>
using namespace axon;

namespace xpb {
//proto package ID
const int PKG_NODE_MSG = 0;

//proto id name define

const int PT_CLIENT_UNICAST = 3;
const int PT_CLIENT_REQ = 2;
const int PT_CGATE_CTRL = 1;


class pb_client_unicast : public proto_msg_t
{
public:
	enum {
		
	};
public:
	static const int proto_id = 3;
	pb_client_unicast () {
		hid = 0;
		main_proto = 0;
		sub_proto = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int32_t hid;
	int16_t main_proto;
	int16_t sub_proto;
	
};


class pb_client_req : public proto_msg_t
{
public:
	enum {
		
	};
public:
	static const int proto_id = 2;
	pb_client_req () {
		hid = 0;
		main_proto = 0;
		sub_proto = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int32_t hid;
	int16_t main_proto;
	int16_t sub_proto;
	
};


class pb_cgate_ctrl : public proto_msg_t
{
public:
	enum {
		
	};
public:
	static const int proto_id = 1;
	pb_cgate_ctrl () {
		cmd = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int16_t cmd;
	
};



} //namespace xpb
#endif
