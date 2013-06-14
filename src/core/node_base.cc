
#include "node_base.h"
#include <core/ax_core.h>
#include <string>
#include <fstream>

namespace axon {

NodeBase::NodeBase(int nodetype)
{
	poller_ = NULL;
	timetick_ = 100;
	node_state_ = ST_CREATE;
	node_type_ = nodetype;
	max_poller_fd_ = MAX_NODE_CNT + 2;
	node_state_ = ST_CREATE;
}

NodeBase::~NodeBase()
{

}


int NodeBase::load_node_config(const char* file, bool is_reload)
{
	Json::Value root;
	Json::Reader reader;
	std::ifstream fs_conf(file, std::ifstream::binary);
	bool parse_ok = reader.parse(fs_conf, root);
	if (!parse_ok) return AX_RET_ERROR;
	if (is_reload) {
		return init_config(root);
	} else {
		return reload_config(root);
	}
		
	return AX_RET_OK;
}

//init base
int NodeBase::init_basic(Json::Value &root)
{
	name_ = root.get("name", "unknown").asString();
	hostnum_ = root.get("hostnum", INTERNAL_HOSTNUM).asInt();
	logpath = root.get("logpath", "./").asString();
	timetick_ = root.get("timetick", 100).asInt();
	nodenum_ = root.get("nodenum", 2).asInt();
}

//init basic net poller(without client port)
int NodeBase::init_inner_net_conf(Json::Value &root)
{
	Json::Value net;
	net = root.get("net", 0);
	if (!net.isObject()) return AX_RET_ERROR;

	inner_port_ = root.get("inner_port", 8883).asInt();
}

int NodeBase::init_outer_net_conf(Json::Value &root)
{

}

int NodeBase::init_config(Json::Value &root)
{
	int ret;
	ret = init_basic(root);
	if (ret == AX_RET_ERROR) return ret;
	ret = init_inner_net_conf(root);
	if (ret == AX_RET_ERROR) return ret;
	ret = init_outer_net_conf(root);
	if (ret == AX_RET_ERROR) return ret;

	poller_ = new EvPoller(max_poller_fd_, timetick_);
}

int NodeBase::reload_config(Json::Value &root)
{

}

int NodeBase::run()
{

}

int NodeBase::terminate()
{
	abort();
}




}  //namespace
