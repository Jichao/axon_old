
#include <core/ax_core.h>
#include "node_init.h"
using namespace axon;

int main()
{
	RawLogger::set_pathroot("logs/");
	NodeConf::set_node_config_file("cgate.conf");
	NodeConf::load_node_conf(0);	
	return 0;
}
