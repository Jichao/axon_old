//Purpose: app_node Init and global variable Config

#include "node_init.h"
#include <jsoncpp/json.h>
#include <core/ax_core.h>
#include <string>
#include <fstream>

using namespace axon;

static char g_conf_file[512] = "cgate.conf";
ClientMgr *g_client_mgr = NULL;
EvPoller *g_main_poller = NULL;

//global var
string_t NodeConf::name("");
int NodeConf::hostnum = 10;
string_t NodeConf::logpath("");
string_t NodeConf::workdir("");
int NodeConf::timetick = 100;
//net config


void NodeConf::set_node_config_file(const char* file)
{
	memset(g_conf_file, 0, sizeof(g_conf_file));
	strncpy(g_conf_file, file, sizeof(g_conf_file) - 1);
}


int NodeConf::init_node_conf(Json::Value *root)
{
	Json::Value net;
	if (root == NULL) return AX_RET_ERROR;

	name.assign(root->get("name", "unknown").asString().c_str());
	hostnum = root->get("hostnum", 89).asInt();
	timetick = root->get("timetick", 100).asInt();
	net = root->get("net", 0);
	if (net.isObject()) {
		int port, backlog, max_conn;
		int rsize;
		port = net.get("client_port", 1234).asInt();
		backlog = net.get("backlog", 10).asInt();
		max_conn = net.get("max_connect", 1000).asInt();
		rsize = net.get("client_rbuf", 1000).asInt();
		g_main_poller = new EvPoller(max_conn, timetick);
		g_client_mgr = new ClientMgr(g_main_poller);
		g_client_mgr->init(max_conn, rsize, 100, port, backlog);
	} else {
		return AX_RET_ERROR;
	}

	logpath.assign(root->get("logpath", "./").asString().c_str());
	workdir.assign(root->get("workdir", "./").asString().c_str());

	return AX_RET_OK;
}

//reload config during runtime
//reset some of the global setting and do some reinit work
void NodeConf::reload_node_conf(Json::Value *root)
{


}

//init global variable when node process start
int NodeConf::load_node_conf(int is_reload)
{
	Json::Value root;
	Json::Reader reader;		
	std::ifstream fs_conf(g_conf_file, std::ifstream::binary);
	bool parse_ok = reader.parse(fs_conf, root);
	if (!parse_ok) {
		//printf("parse fail. %s", reader.getFormatedErrorMessages().c_str());
		return -1;
	}

	//init node's global data
	if (is_reload == 0) {
		return init_node_conf(&root);
	} else {
		reload_node_conf(&root);
	}
	return 0;
}

