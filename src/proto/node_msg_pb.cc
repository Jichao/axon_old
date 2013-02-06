//============================================
//proto_msg definition
//Warning:This file is auto generated. Don't modify it by hand
//============================================

#include "node_msg_pb.h"

namespace xpb {


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_client_unicast::cal_size()
{
	uint32_t pl_len = 0;
	if (pl.data != NULL) {
		if (pl.from_unpack == 0) {
			pl.pl_len = ((proto_msg_t*)(pl.data))->cal_size();
		}
		pl_len += pl.pl_len;
	}

	return 8 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_client_unicast::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 8;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	//little endian only (x86)
	hid = *(int*)p;
	p += 4;
	_shift += 4;
	pl.proto = *p | (*(p+1) << 8);
	pl.pl_len = *(p+2) | (*(p+3) << 8);
	p += 4;
	_shift += 4;
	if (pl.pl_len > avail - _shift) return -1;
	pl.data = p;
	pl.from_unpack = 1;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_client_unicast::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 8;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	//little endian only (x86)
	*(int*)p = hid;
	p += 4;
	_shift += 4;
	*p = pl.proto & 0xFF;
	*(p+1) = (pl.proto >> 8) & 0xFF;
	ptmp = p;  //pack length later
	p += 4;
	_shift += 4;
	pl.pl_len = 0;
	//pack payload
	if (pl.data != NULL && pl.from_unpack != 1) {
		ret = ((proto_msg_t*)(pl.data))->pack(p + _shift, avail - _shift);
		if (ret < 0) return -1;
		pl.pl_len = ret;
		_shift += ret;
	}
	//pack length
	*ptmp = pl.pl_len & 0xFF;
	*(ptmp + 1) = (pl.pl_len >> 8) & 0xFF;

	return _shift;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_client_req::cal_size()
{
	uint32_t pl_len = 0;
	if (pl.data != NULL) {
		if (pl.from_unpack == 0) {
			pl.pl_len = ((proto_msg_t*)(pl.data))->cal_size();
		}
		pl_len += pl.pl_len;
	}

	return 8 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_client_req::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 8;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	//little endian only (x86)
	hid = *(int*)p;
	p += 4;
	_shift += 4;
	pl.proto = *p | (*(p+1) << 8);
	pl.pl_len = *(p+2) | (*(p+3) << 8);
	p += 4;
	_shift += 4;
	if (pl.pl_len > avail - _shift) return -1;
	pl.data = p;
	pl.from_unpack = 1;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_client_req::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 8;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	//little endian only (x86)
	*(int*)p = hid;
	p += 4;
	_shift += 4;
	*p = pl.proto & 0xFF;
	*(p+1) = (pl.proto >> 8) & 0xFF;
	ptmp = p;  //pack length later
	p += 4;
	_shift += 4;
	pl.pl_len = 0;
	//pack payload
	if (pl.data != NULL && pl.from_unpack != 1) {
		ret = ((proto_msg_t*)(pl.data))->pack(p + _shift, avail - _shift);
		if (ret < 0) return -1;
		pl.pl_len = ret;
		_shift += ret;
	}
	//pack length
	*ptmp = pl.pl_len & 0xFF;
	*(ptmp + 1) = (pl.pl_len >> 8) & 0xFF;

	return _shift;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_cgate_ctrl::cal_size()
{
	uint32_t pl_len = 0;
	if (pl.data != NULL) {
		if (pl.from_unpack == 0) {
			pl.pl_len = ((proto_msg_t*)(pl.data))->cal_size();
		}
		pl_len += pl.pl_len;
	}

	return 4 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_cgate_ctrl::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 4;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	pl.proto = *p | (*(p+1) << 8);
	pl.pl_len = *(p+2) | (*(p+3) << 8);
	p += 4;
	_shift += 4;
	if (pl.pl_len > avail - _shift) return -1;
	pl.data = p;
	pl.from_unpack = 1;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_cgate_ctrl::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 4;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	*p = pl.proto & 0xFF;
	*(p+1) = (pl.proto >> 8) & 0xFF;
	ptmp = p;  //pack length later
	p += 4;
	_shift += 4;
	pl.pl_len = 0;
	//pack payload
	if (pl.data != NULL && pl.from_unpack != 1) {
		ret = ((proto_msg_t*)(pl.data))->pack(p + _shift, avail - _shift);
		if (ret < 0) return -1;
		pl.pl_len = ret;
		_shift += ret;
	}
	//pack length
	*ptmp = pl.pl_len & 0xFF;
	*(ptmp + 1) = (pl.pl_len >> 8) & 0xFF;

	return _shift;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_wrapper::cal_size()
{
	uint32_t pl_len = 0;
	if (pl.data != NULL) {
		if (pl.from_unpack == 0) {
			pl.pl_len = ((proto_msg_t*)(pl.data))->cal_size();
		}
		pl_len += pl.pl_len;
	}

	return 4 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_wrapper::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 4;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	pl.proto = *p | (*(p+1) << 8);
	pl.pl_len = *(p+2) | (*(p+3) << 8);
	p += 4;
	_shift += 4;
	if (pl.pl_len > avail - _shift) return -1;
	pl.data = p;
	pl.from_unpack = 1;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_wrapper::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 4;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	*p = pl.proto & 0xFF;
	*(p+1) = (pl.proto >> 8) & 0xFF;
	ptmp = p;  //pack length later
	p += 4;
	_shift += 4;
	pl.pl_len = 0;
	//pack payload
	if (pl.data != NULL && pl.from_unpack != 1) {
		ret = ((proto_msg_t*)(pl.data))->pack(p + _shift, avail - _shift);
		if (ret < 0) return -1;
		pl.pl_len = ret;
		_shift += ret;
	}
	//pack length
	*ptmp = pl.pl_len & 0xFF;
	*(ptmp + 1) = (pl.pl_len >> 8) & 0xFF;

	return _shift;
}



} //namespace
