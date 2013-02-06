//top level message between nodes
//write this file by hand

#include "node_msg.pb.h"

//return self size of unpack
//return -1 if error
int pb_node_msg::unpack(char* p, uint32_t avail)
{
	int _sl = 4;
	int _shift = 0;
	char* phead = p;
	if (avail < _sl) return -1;
	pl_len = *p | (*(p+1) << 8);
	p += 2;
	pl.proto = *p | (*(p+1) << 8);
	p += 2;
	pl.data = NULL;

	_shift += _sl;
	return _shift;
}

//return total size of packed data
//return -1 if error
int pb_node_msg::pack(char* p, uint32_t avail)
{
	int _sl = 4;
	int ret;
	if (pl.data != NULL) {
		ret = pl.data->pack(p + _sl, avail - _sl);
		if (ret < 0) return -1;
		pl_len = ret;
	}
	*p++ = pl_len & 0xFF;
	*p++ = (pl_len >> 8	) & 0xFF;
	*p++ = pl.proto & 0xFF;
	*p++ = (pl.proto >> 8) & 0xFF;
	return _sl + pl_len;
}

