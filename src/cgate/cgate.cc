
#include <core/ax_core.h>
#include "node_init.h"
using namespace axon;

int main()
{
	RawLogger::set_pathroot("logs/");
	set_node_config_file("cgate.conf");
	load_node_conf(0);	
	return 0;
}
