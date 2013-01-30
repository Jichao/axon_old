
#include <core/ax_core.h>
#include "node_init.h"
using namespace axon;

int main()
{
	int ret;
	RawLogger::set_pathroot("./log/");
	NodeConf::set_node_config_file("cgate.conf");
	ret = NodeConf::load_node_conf(0);	
	if (ret == AX_RET_ERROR) return 0;

	if (g_client_mgr->start_worker() == AX_RET_ERROR) return 0;
	ret = g_client_mgr->listen();
	if (ret == AX_RET_ERROR) return 0;
	g_client_mgr->process();
	return 0;
}


