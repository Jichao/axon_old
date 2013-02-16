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
	"proto_id": 3, 
	"desc": "new connection",
	"func": "",
	"body": [
		("cmd", INT16),
		("lparam", INT32),
		("wparam", INT16), 
	],
	"const": {
		"WORKER_START_OK": 1, 

	}
},

"conn_notify" : {
	"proto_id": 4,
	"desc": "connection notify by worker",
	"func": "",
	"body": [
		("fd", INT32),
		("hid", INT32),
		("action", INT8),	
	],
	"const": {
		"PEER_CLOSE": 1, 
		"PEER_TIMEOUT": 2, 
	}
},

"conn_ctrl" : {
	"proto_id": 5,
	"desc": "connection ctrl by main thread",
	"func": "",
	"body": [
		("fd", INT32),
		("hid", INT32),
		("action", INT8),	
	],
	"const": {
		"DO_CLOSE": 1, 
	}
},

}

