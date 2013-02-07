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
	return 8 ;
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
	main_proto = *(short*)p;
	p += 2;
	sub_proto = *(short*)p;
	p += 2;

	return _shift + _fl;
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
	*(short*)p = main_proto;
	p += 2;
	*(short*)p = sub_proto;
	p += 2;

	return _shift + _fl;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_client_req::cal_size()
{
	return 8 ;
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
	main_proto = *(short*)p;
	p += 2;
	sub_proto = *(short*)p;
	p += 2;

	return _shift + _fl;
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
	*(short*)p = main_proto;
	p += 2;
	*(short*)p = sub_proto;
	p += 2;

	return _shift + _fl;
}


//call this function before pack to prepare buffer
//don't call it after unpack for proto which has a payload field
int pb_cgate_ctrl::cal_size()
{
	return 2 ;
}

//return actual shift size
//       -1 if error
int pb_cgate_ctrl::unpack(char* p, uint32_t avail)
{
	uint32_t _fl = 2;
	int _shift = 0;
	if ( p == NULL || avail < _fl ) return -1;
	cmd = *(short*)p;
	p += 2;

	return _shift + _fl;
}

//return total shift size
//       -1 if error
int pb_cgate_ctrl::pack(char* p, uint32_t avail)
{
	uint32_t _fl = 2;
	int _shift = 0;
	int ret;
	char* ptmp;
	if ( p == NULL || avail < _fl) return -1;
	*(short*)p = cmd;
	p += 2;

	return _shift + _fl;
}



} //namespace
