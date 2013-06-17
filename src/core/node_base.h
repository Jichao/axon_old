//base class of node
// Author: Daly

#ifndef _NODE_BASE_H_
#define _NODE_BASE_H_

#include <jsoncpp/json.h>
#include "fd_poller.h"

namespace axon 
{


enum NodeType {
	T_NODE_MASTER = 1,
	T_NODE_CGATE = 2,
	T_NODE_PGATE = 3,
	T_NODE_LOGIC = 4,
	T_NODE_AUX = 5,
};

enum NodeState {
	ST_CREATE = 1,
	ST_CONF_LOADED = 2,
	ST_RUNNING = 2,
} ;

class NodeBase
{
public:
	NodeBase(int nodetype);
	~NodeBase();

	//init config
	int load_node_config(const char* file, bool is_reload);
	int init_basic(Json::Value &root);   //basic node attribute
	virtual int init_inner_net_conf(Json::Value &root);
	virtual int init_outer_net_conf(Json::Value &root);

	virtual int init_config(Json::Value &root);
	virtual int reload_config(Json::Value &root);


	virtual int run();
	virtual int terminate();

protected:
	int node_type_; 
	int node_state_;
	EvPoller *poller_;   //main event poller

public:
	//Node common attribute
	std::string name_;  
	int nodenum_;    //node number in cluster
	int hostnum_;
	std::string logpath;
	int timetick_;   //event framerate
	int max_poller_fd_;	
	int inner_port_;   //inner listen port
};


} //namespace axon


#endif
