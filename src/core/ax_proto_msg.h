//message protocol base between node/process/threads
//like google protobuf
//
#ifndef _AX_PROTO_MSG_H_
#define _AX_PROTO_MSG_H_

#include "stdheader.h"

namespace axon {

#define MAX_PROTO_LEN  32000

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

//variable len payload_t
struct pb_payload_t
{
	pb_payload_t() { 
		proto=0; 
		pl_len = 0;
		data = NULL; 
		from_unpack = 0;
	}
	char from_unpack;   //make sure the data is valid proto_msg_t
	uint16_t proto;
	uint16_t pl_len;    //payload len
	char* data;         //when pack: convertion to proto_msg_t* 
						//when unpack: point to temp buffer (never reference it elsewhere after packet unpack/processed )
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
