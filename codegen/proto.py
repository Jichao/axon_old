from prot_basedef import *
import jinja2
import sys, os
import types
import proto_gen_cpp as gen_cpp

TEMPLATE_PATH = 'template/'
SCHEMA_PATH = '../schema/proto/'
OUTPUT_CPP  = '../src/proto/'

sys.path.append(SCHEMA_PATH)

req_field = ['body', 'proto_id', 'desc', 'func', ]

g_cpp_mod = []

def output_engine(pkg_info, proto_def):
	ctx = {}
	pkg_name = pkg_info['pkg_name']
	ctx['pkg_name'] = pkg_name
	ctx['pkg_id_name'] = 'PKG_' + pkg_name.upper()
	ctx['pkg_id'] = pkg_info['pkg_id']

	for pname, v in proto_def.items():
		gen_cpp.gen_cpp_body(pname, v)	

	gen_cpp.gen_pkg_instance(ctx, proto_def)
	ctx['all_proto'] = proto_def

	fileloader = jinja2.FileSystemLoader(TEMPLATE_PATH)
	env = jinja2.Environment(loader = fileloader)
	#render header
	tpl = env.get_template('proto_cc.h.tmpl')
	output = tpl.render(ctx)
	f = open(OUTPUT_CPP + pkg_name + '_pb.h', 'w')
	f.write(output)
	f.close()
	#render source
	tpl = env.get_template('proto_cc.cc.tmpl')
	output = tpl.render(ctx)
	f = open(OUTPUT_CPP + pkg_name + '_pb.cc', 'w')
	f.write(output)
	f.close()
	

def output_script(pkg_info, proto_def):
	pass


#load proto schema
def process_schema(mod_name):
	m = __import__(mod_name)
	mp = {}
	proto_def = {}
	try:
		mp['pkg_name'] = mod_name.split('_pb')[0]
		mp['pkg_id'] = m.__dict__['PKG_ID']
		mp['output_engine'] = m.__dict__['OUTPUT_ENGINE']
		mp['output_script'] = m.__dict__['OUTPUT_SCRIPT']
		proto_def = m.__dict__['PROTO_DEF']
	except:
		print 'compile error'
		raise

	try:
		if mp['output_engine'] == 1:
			output_engine(mp, proto_def)
			g_cpp_mod.append(mp['pkg_name'])

		if mp['output_script'] == 1:
			output_script(mp, proto_def)
	except:
		print 'compile %s error.' % mod_name


def get_all_schema():
	ret = []
	file_list = os.listdir(SCHEMA_PATH)
	for filename in file_list:
		if not filename.endswith('_pb.py'): continue
		ret += [filename.split('.py')[0], ]
	return ret

if __name__== '__main__':
	all_prot = get_all_schema()
	for mod_name in all_prot:
		process_schema(mod_name)
	
	#generate makefile
	obj_list = ''
	dep_list = ''
	for mod_name in g_cpp_mod:
		obj_list += '%s_pb.o ' % mod_name
		dep_list += '%s_pb.h ' % mod_name
	
	fileloader = jinja2.FileSystemLoader(TEMPLATE_PATH)
	env = jinja2.Environment(loader = fileloader)

	tpl = env.get_template('proto_makefile.tmpl')	
	output = tpl.render(deps=dep_list, objs=obj_list)
	f = open(OUTPUT_CPP + 'Makefile.dep', 'w')
	f.write(output)
	f.close()
	print 'compile proto complete\n'
