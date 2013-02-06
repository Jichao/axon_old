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

const int PT_CLIENT_UNICAST = 4;
const int PT_CLIENT_REQ = 3;
const int PT_CGATE_CTRL = 2;
const int PT_WRAPPER = 0;



class pb_client_unicast : public proto_msg_t
{
public:
	static const int proto_id = 4;
	pb_client_unicast () {
		hid = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int32_t hid;
	pb_payload_t pl;
	
};


class pb_client_req : public proto_msg_t
{
public:
	static const int proto_id = 3;
	pb_client_req () {
		hid = 0;
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	int32_t hid;
	pb_payload_t pl;
	
};


class pb_cgate_ctrl : public proto_msg_t
{
public:
	static const int proto_id = 2;
	pb_cgate_ctrl () {
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	pb_payload_t pl;
	
};


class pb_wrapper : public proto_msg_t
{
public:
	static const int proto_id = 0;
	pb_wrapper () {
		
	}
	virtual int cal_size();
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);

	pb_payload_t pl;
	
};



} //namespace xpb
#endif
