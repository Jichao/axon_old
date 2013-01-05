
#ifndef _NODE_INIT_H_
#define _NODE_INIT_H_

#include <core/ax_core.h>
#include <jsoncpp/json.h>
#include "client_mgr.h"

using namespace axon;

//client manager
extern ClientMgr *g_client_mgr;

class NodeConf 
{
public:
	static void set_node_config_file(const char* file);
	static int load_node_conf(int is_reload);
private:
	static int init_node_conf(Json::Value *root);
	static void reload_node_conf(Json::Value *root);

public:
	static string_t name;
	static int hostnum;
	static string_t logpath;
	static string_t workdir;
	static int timetick;
};

#endif
