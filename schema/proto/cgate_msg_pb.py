from prot_basedef import *

PKG_ID = 1

OUTPUT_ENGINE = 1
OUTPUT_SCRIPT = 0

#====PROTO_DEF=======================
PROTO_DEF = {

"cgate_newconn" : {
	"proto_id": 1, 
	"desc": "new connection",
	"func": "",
	"body": [
		("fd", INT32),
		("hid", UINT32), 
		("ip", UINT64),
		("port", UINT16),
	]
},

"worker_ctrl" : {
	"proto_id": 2, 
	"desc": "new connection",
	"func": "",
	"body": [
		("cmd", INT16),
		("lparam", INT32),
	]
},

"worker_notify" : {
	"proto_id": 2, 
	"desc": "new connection",
	"func": "",
	"body": [
		("cmd", INT16),
		("lparam", INT32),
	]
},


}

