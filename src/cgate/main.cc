
#include <core/ax_core.h>
#include "node_def.h"
#include "node_init.h"
using namespace axon;

int g_node_state = 0;

int main()
{
	int ret;

	debug_log("[INFO] node process started");
	NodeConf::set_node_config_file("cgate.conf");
	ret = NodeConf::load_node_conf(0);	
	if (ret == AX_RET_ERROR) {
		debug_log("[ERROR] failed to load config file.");
		return 0;
	}

	RawLogger::set_pathroot(NodeConf::logpath.c_str());
	debug_log("node process exit");
	return 0;
}


