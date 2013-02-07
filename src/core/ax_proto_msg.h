//message protocol base between node/process/threads
//like google protobuf
//
#ifndef _AX_PROTO_MSG_H_
#define _AX_PROTO_MSG_H_

#include "stdheader.h"

namespace axon {

#define MAX_PROTO_LEN  32000
#define MSG_HEADER_LEN 4
#define UNPACK_HEADER(h, p)  (h)->proto = *(p) | *((p)+1) << 8; \
	(h)->pl_len = *((p)+2) | *((p)+3) << 8;

#define PACK_HEADER(h, p) *p = (h)->proto & 0xFF; \
	*(p+1) = ((h)->proto >> 8) & 0xFF; \
	*(p+2) = (h)->pl_len & 0xFF; \
	*(p+3) = ((h)->pl_len >> 8) & 0xFF;
	
struct msg_header_t
{
	uint16_t proto;   //wrapper proto id
	uint16_t pl_len;      //payload len
};


//base class of all message
class proto_msg_t 
{
public:
	proto_msg_t() {}

	//call this function before pack to prepare buffer
	////don't call it after unpack for proto which has a payload field
	virtual int cal_size() = 0;
	virtual int unpack(char* p, uint32_t avail) = 0;
	virtual int pack(char* p, uint32_t maxn) = 0;
};

struct raw_bytes_t {
	raw_bytes_t() { 
		len = 0; 
		data = NULL; 
	}
	uint16_t len;
	char* data;
};

} //namespace

#endif
