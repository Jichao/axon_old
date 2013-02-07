from prot_basedef import *

#message between nodes

PKG_ID = 0

OUTPUT_ENGINE = 1
OUTPUT_SCRIPT = 0

#====PROTO_DEF=======================
PROTO_DEF = {

"cgate_ctrl" : {
	"proto_id": 1, 
	"desc": "cgate ctrl",
	"func": "",
	"body": [
		("cmd", INT16),
	]
},

"client_req" : {
	"proto_id": 2,
	"desc": "client packet wrapper",
	"body": [
		("hid", INT32),
		("main_proto", INT16),
		("sub_proto", INT16),
	]
},

"client_unicast" : {
	"proto_id": 3,
	"desc": "wrapper of unicast data",
	"body": [
		("hid", INT32),
		("main_proto", INT16),
		("sub_proto", INT16),
	]
}

}

