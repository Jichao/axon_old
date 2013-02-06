//root node message definition (engine level)
//the top level protocol defined by hard code.

#ifndef _NODE_MSG_PB_H_
#define _NODE_MSG_PB_H_

using namespace axon;

#define PT_CLIENT_SC         1  //unicast to client
#define PT_CLIENT_SC_MULTI   2  //multicast/broadcast to CS
#define PT_CLIENT_CS         3  //proto recv from client
#define PT_HEARTBEAT         4  //common heartbeat
#define PT_GATE_WORKER       5  //ctrl between worker threads in cgate


class msg_gate_worker : public proto_msg_t
{
public:
	msg_gate_worker() { proto_id_ = PT_GATE_WORKER; }
	
}





#endif
