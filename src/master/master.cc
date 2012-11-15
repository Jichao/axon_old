
#include <core/ax_core.h>

using namespace axon;

int main(int argc, char** argv)
{
	debug_log("game server started");
	while ((ch = getopt(argc, argv, ":c")) != -1)
	{
		switch(ch)
		{
		case 'c':
			return 0;
		default:
			break;

		}


	}
	debug_log("game server exit");
	return 0;
}

