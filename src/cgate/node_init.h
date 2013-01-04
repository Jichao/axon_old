
#ifndef _NODE_INIT_H_
#define _NODE_INIT_H_

#include <jsoncpp/json.h>

class NodeConf 
{
public:
	static void set_node_config_file(const char* file);
	static int load_node_conf(int is_reload);
private:
	static void init_node_conf(Json::Value *root);
	static void reload_node_conf(Json::Value *root);
};

#endif
