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
}

g_field_len = {
	INT8: 1,
	INT16: 2, 
	INT32: 4,
	UINT8: 1, 
	UINT16: 2,
	UINT32: 4, 
	UINT64: 8, 
	VARSTR: 2, 
}

#pack string

PACK_INT8 = '''\
	*p++ = %s & 0xFF;
'''

PACK_INT16 = '''\
	*(short*)p = %s;
	p += 2;
'''

PACK_INT32 = '''\
	//little endian only (x86)
	*(int*)p = %s;
	p += 4;
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
'''


PACK_VARSTR = '''\
	int %s_slen = %s.len() + 1
	if (%s_slen + 2 > avail - _shift) return -1;
	//pack length first
	*p++ = %s_slen & 0xFF;
	*p++ = (%s_slen >> 8) & 0xFF;
	::strcpy(p, %s.c_str())
	p += %s_slen;
	_shift += %s_slen;

'''


UNPACK_INT8 = '''\
	%s = *p++;
'''
UNPACK_INT16 = '''\
	%s = *(short*)p;
	p += 2;
'''
UNPACK_INT32 = '''\
	//little endian only (x86)
	%s = *(int*)p;
	p += 4;
'''

UNPACK_INT64 = '''
	%s = *p | (*(p+1) << 8);
	%s |= *(p+2) << 16 | (*(p+3) << 24);
	p += 4;
	%s |= *p | (*(p+1) << 8);
	%s |= *(p+2) << 16 | (*(p+3) << 24);
	p += 4;
'''

UNPACK_VARSTR = '''\
	int %s_slen = *p | (*(p+1) << 8);
	p += 2;
	if (%s_slen == 0 || %s_slen + _shift > avail - _fl) return -1;
	*(p + %s_slen - 1) = '\\0';   //be sure 0 
	%s.assign(p);
	_shift += %s_slen;
	p += %s_slen;
'''

g_mp_code = {
	INT8: (PACK_INT8, UNPACK_INT8),
	UINT8: (PACK_INT8, UNPACK_INT8),
	INT16: (PACK_INT16, UNPACK_INT16),
	UINT16: (PACK_INT16, UNPACK_INT16),
	INT32: (PACK_INT32, UNPACK_INT32),
	UINT32: (PACK_INT32, UNPACK_INT32),
	UINT64: (PACK_INT64, UNPACK_INT64),
	VARSTR: (PACK_VARSTR, UNPACK_VARSTR),
}

def gen_pkg_instance(ctx, proto_def):
	max_proto = 0
	create_s = ''
	for pname, v in proto_def.items():
		if max_proto < v['proto_id']:
			max_proto = v['proto_id']
		create_s += '\tinstance_[%d] = new %s;\n' % (v['proto_id'], 'pb_'+pname)

	ctx['pkg_create_instance'] = create_s
	ctx['pkg_max_proto'] = max_proto + 1

def gen_cpp_body(pname, vdef):
	body = vdef['body']
	init_field = ''
	decl_field = ''
	unpack_str = ''
	pack_str = ''
	len_str = ''
	var_len = ''
	enum_str = ''
	fixed_len = 0
	ftype = 0
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

	for ctx in body:
		k = ctx[0]
		ftype = ctx[1]

		if ftype not in cpp_field: continue
		decl_field += '%s %s;\n\t' % (cpp_field[ftype], k)

		if ftype in zero_default:
			init_field += '%s = 0;\n\t\t' % k

		if ftype == VARSTR:
			var_len += "+ %s.len()" % k

		unpack_str += g_mp_code[ftype][1].replace('%s', k)
		pack_str += g_mp_code[ftype][0].replace('%s', k)

	vdef['fixed_len'] = len_str
	vdef['var_len'] = var_len
	vdef['unpack'] = unpack_str
	vdef['pack'] = pack_str
	vdef['decl_field'] = decl_field
	vdef['init_field'] = init_field
	vdef['id_name'] = 'PT_' + pname.upper()
	vdef['enum'] = ''
	if "const" in vdef:
		for k, v in vdef['const'].items():
			vdef['enum'] += '%s = %d,\n\t\t' % (k, v)

