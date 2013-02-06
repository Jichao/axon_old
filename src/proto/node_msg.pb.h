//the top level message between nodes
//this file is write by hand

#ifndef _NODE_MSG_PB_
#define _NODE_MSG_PB_

#include <ax_core/core>
using namespace axon;

class pb_node_msg : public proto_msg_t
{
public:
	pb_node_msg() { 
		pl_len = 0;
	}
	virtual int unpack(char* p, uint32_t avail);
	virtual int pack(char* p, uint32_t maxn);
	
	uint16_t pl_len;
	struct pb_payload_t pl;
}



#endif
