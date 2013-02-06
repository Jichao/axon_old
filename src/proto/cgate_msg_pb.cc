//============================================
//proto_msg definition
//Warning:This file is auto generated. Don't modify it by hand
//============================================

#include "cgate_msg_pb.h"

namespace xpb {


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_cgate_newconn::cal_size()
{
	uint32_t pl_len = 0;

	return 18 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_cgate_newconn::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 18;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	//little endian only (x86)
	fd = *(int*)p;
	p += 4;
	_shift += 4;
	//little endian only (x86)
	hid = *(int*)p;
	p += 4;
	_shift += 4;

	ip = *p | (*(p+1) << 8);
	ip |= *(p+2) << 16 | (*(p+3) << 24);
	p += 4;
	ip |= *p | (*(p+1) << 8);
	ip |= *(p+2) << 16 | (*(p+3) << 24);
	p += 4;
	_shift += 4;
	port = *(short*)p;
	p += 2;
	_shift += 2;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_cgate_newconn::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 18;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	//little endian only (x86)
	*(int*)p = fd;
	p += 4;
	_shift += 4;
	//little endian only (x86)
	*(int*)p = hid;
	p += 4;
	_shift += 4;
	*p++ = ip & 0xFF;
	*p++ = (ip >> 8) & 0xFF;
	*p++ = (ip >> 16) & 0xFF;
	*p++ = (ip >> 24) & 0xFF;
	*p++ = (ip >> 32) & 0xFF;
	*p++ = (ip >> 40) & 0xFF;
	*p++ = (ip >> 48) & 0xFF;
	*p++ = (ip >> 56) & 0xFF;
	_shift += 8;
	*(short*)p = port;
	p += 2;
	_shift += 2;

	return _shift;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_worker_ctrl::cal_size()
{
	uint32_t pl_len = 0;

	return 6 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_worker_ctrl::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 6;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	cmd = *(short*)p;
	p += 2;
	_shift += 2;
	//little endian only (x86)
	lparam = *(int*)p;
	p += 4;
	_shift += 4;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_worker_ctrl::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 6;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	*(short*)p = cmd;
	p += 2;
	_shift += 2;
	//little endian only (x86)
	*(int*)p = lparam;
	p += 4;
	_shift += 4;

	return _shift;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_worker_notify::cal_size()
{
	uint32_t pl_len = 0;

	return 6 + pl_len;
}

//return actual shift size
//       -1 if error
int pb_worker_notify::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 6;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	cmd = *(short*)p;
	p += 2;
	_shift += 2;
	//little endian only (x86)
	lparam = *(int*)p;
	p += 4;
	_shift += 4;

	return _shift;
}

//return total shift size
//       -1 if error
int pb_worker_notify::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 6;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	*(short*)p = cmd;
	p += 2;
	_shift += 2;
	//little endian only (x86)
	*(int*)p = lparam;
	p += 4;
	_shift += 4;

	return _shift;
}



} //namespace
