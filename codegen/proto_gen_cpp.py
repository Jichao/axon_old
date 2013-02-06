from prot_basedef import *
zero_default = [INT8, INT16, INT32, UINT8, UINT16, UINT32, UINT64, ]

cpp_field = {
	INT8: "int8_t", 
	INT16: "int16_t",
	INT32: "int32_t",
	UINT8: "uint8_t",
	UINT16: "uint16_t",
	UINT32: "uint32_t",
	UINT64: "uint64_t",
	VARSTR: "string_t",
	PAYLOAD: "pb_payload_t",
}

g_field_len = {
	INT8: 1,
	INT16: 2, 
	INT32: 4,
	UINT8: 1, 
	UINT16: 2,
	UINT32: 4, 
	UINT64: 8, 
	PAYLOAD: 4,
	VARSTR: 3, 
}

#pack string

PACK_INT8 = '''\
	*p++ = %s & 0xFF;
	_shift += 1;
'''

PACK_INT16 = '''\
	*(short*)p = %s;
	p += 2;
	_shift += 2;
'''

PACK_INT32 = '''\
	//little endian only (x86)
	*(int*)p = %s;
	p += 4;
	_shift += 4;
'''

PACK_INT64 = '''\
	*p++ = %s & 0xFF;
	*p++ = (%s >> 8) & 0xFF;
	*p++ = (%s >> 16) & 0xFF;
	*p++ = (%s >> 24) & 0xFF;
	*p++ = (%s >> 32) & 0xFF;
	*p++ = (%s >> 40) & 0xFF;
	*p++ = (%s >> 48) & 0xFF;
	*p++ = (%s >> 56) & 0xFF;
	_shift += 8;
'''

PACK_PAYLOAD = '''\
	*p = %s.proto & 0xFF;
	*(p+1) = (%s.proto >> 8) & 0xFF;
	ptmp = p;  //pack length later
	p += 4;
	_shift += 4;
	%s.pl_len = 0;
	//pack payload
	if (%s.data != NULL && %s.from_unpack != 1) {
		ret = ((proto_msg_t*)(%s.data))->pack(p + _shift, avail - _shift);
		if (ret < 0) return -1;
		%s.pl_len = ret;
		_shift += ret;
	}
	//pack length
	*ptmp = %s.pl_len & 0xFF;
	*(ptmp + 1) = (%s.pl_len >> 8) & 0xFF;
'''

PACK_VARSTR = '''\
	int %s_slen = %s.len() + 1
	if (%s_slen + 2 > avail - _shift) return -1;
	//pack length first
	*p++ = %s_slen & 0xFF;
	*p++ = (%s_slen >> 8) & 0xFF;
	_shift += 2;
	::strcpy(p, %s.c_str())
	p += %s_slen;
	_shift += %s_slen;

'''


UNPACK_INT8 = '''\
	%s = *p++;
	_shift += 1;
'''
UNPACK_INT16 = '''\
	%s = *(short*)p;
	p += 2;
	_shift += 2;
'''
UNPACK_INT32 = '''\
	//little endian only (x86)
	%s = *(int*)p;
	p += 4;
	_shift += 4;
'''

UNPACK_INT64 = '''
	%s = *p | (*(p+1) << 8);
	%s |= *(p+2) << 16 | (*(p+3) << 24);
	p += 4;
	%s |= *p | (*(p+1) << 8);
	%s |= *(p+2) << 16 | (*(p+3) << 24);
	p += 4;
	_shift += 4;
'''

UNPACK_PAYLOAD = '''\
	%s.proto = *p | (*(p+1) << 8);
	%s.pl_len = *(p+2) | (*(p+3) << 8);
	p += 4;
	_shift += 4;
	if (%s.pl_len > avail - _shift) return -1;
	%s.data = p;
	%s.from_unpack = 1;
'''	

UNPACK_VARSTR = '''\
	int %s_slen = *p | (*(p+1) << 8);
	p += 2;
	_shift += 2;
	if (%s_slen == 0 || %s_slen > avail - _shift) return -1;
	*(p + %s_slen - 1) = '\\0';   //be sure 0 
	%s.assign(p);
	_shift += %s_slen;
	p += %s_slen;
'''

PAYLOAD_CALSIZE = '''\
	if (%s.data != NULL) {
		if (%s.from_unpack == 0) {
			%s.pl_len = ((proto_msg_t*)(%s.data))->cal_size();
		}
		pl_len += %s.pl_len;
	}
'''


g_mp_code = {
	INT8: (PACK_INT8, UNPACK_INT8),
	UINT8: (PACK_INT8, UNPACK_INT8),
	INT16: (PACK_INT16, UNPACK_INT16),
	UINT16: (PACK_INT16, UNPACK_INT16),
	INT32: (PACK_INT32, UNPACK_INT32),
	UINT32: (PACK_INT32, UNPACK_INT32),
	UINT64: (PACK_INT64, UNPACK_INT64),
	PAYLOAD: (PACK_PAYLOAD, UNPACK_PAYLOAD),
	VARSTR: (PACK_VARSTR, UNPACK_VARSTR),
}


def gen_cpp_body(pname, vdef):
	body = vdef['body']
	init_field = ''
	decl_field = ''
	unpack_str = ''
	pack_str = ''
	len_str = ''
	fixed_len = 0
	ftype = 0
	pl_len_str = ''
	#cal fixed len
	for ctx in body:
		ftype = ctx[1]
		
		if ftype in g_field_len:
			fixed_len += g_field_len[ ftype ]
		else:
			pass  #cal string size

	if len_str == '' : len_str = '%d' % fixed_len
	else: len_str += '+ %d ' % fixed_len

	for i in xrange(0, len(body)):
		ftype = body[i][1]
		if ftype == PAYLOAD and i != len(body)-1:
			print 'PAYLOAD must be put at last'
			return -2

	for ctx in body:
		k = ctx[0]
		ftype = ctx[1]

		if ftype not in cpp_field: continue
		decl_field += '%s %s;\n\t' % (cpp_field[ftype], k)

		if ftype in zero_default:
			init_field += '%s = 0;\n\t\t' % k

		if ftype == PAYLOAD:
			pl_len_str += PAYLOAD_CALSIZE.replace('%s', k)
		elif ftype == VARSTR:
			pl_len_str += "pl_len += %s.len();\n\t" % k

		unpack_str += g_mp_code[ftype][1].replace('%s', k)
		pack_str += g_mp_code[ftype][0].replace('%s', k)

	vdef['fixed_len'] = len_str
	vdef['unpack'] = unpack_str
	vdef['pack'] = pack_str
	vdef['decl_field'] = decl_field
	vdef['init_field'] = init_field
	vdef['id_name'] = 'PT_' + pname.upper()
	vdef['cal_payload_len'] = pl_len_str

