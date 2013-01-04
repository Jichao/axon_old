//Purpose: app_node Init and global variable Config

#include "node_init.h"
#include <jsoncpp/json.h>
#include <core/ax_core.h>
#include <string>
#include <fstream>

using namespace axon;

static char g_conf_file[512] = "cgate.conf";

void NodeConf::set_node_config_file(const char* file)
{
	memset(g_conf_file, 0, sizeof(g_conf_file));
	strncpy(g_conf_file, file, sizeof(g_conf_file) - 1);
}


void NodeConf::init_node_conf(Json::Value *root)
{
	std::string name;
	int port;
	if (root == NULL) return;

	name = root->get("name", "unknown").asString();
	port = root->get("client_port", 1234).asInt();
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
		init_node_conf(&root);
	} else {
		reload_node_conf(&root);
	}
	return 0;
}

