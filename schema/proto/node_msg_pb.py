from prot_basedef import *

#message between nodes

PKG_ID = 0

OUTPUT_ENGINE = 1
OUTPUT_SCRIPT = 0

#====PROTO_DEF=======================
PROTO_DEF = {

#the bottom level of engine message
"wrapper" : {
	"dir": N2N, 
	"proto_id": 0, 
	"desc": 'bottom engine',
	"func": '', 
	"body": [
		("pl", PAYLOAD), 
	]
},

"cgate_ctrl" : {
	"dir": G2G,
	"proto_id": 2, 
	"desc": "new connection",
	"func": "",
	"body": [
		("pl", PAYLOAD),
	]
},

"client_req" : {
	"dir": N2N,
	"proto_id": 3,
	"desc": "client packet wrapper",
	"body": [
		("hid", INT32),
		("pl", PAYLOAD),
	]
},

"client_unicast" : {
	"dir": N2N,
	"proto_id": 4,
	"desc": "wrapper of unicast data",
	"body": [
		("hid", INT32),
		("pl", PAYLOAD),
	]
}

}

