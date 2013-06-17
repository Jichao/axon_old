
#include "node_common.h"
#include "ax_core.h"

namespace axon {

static void on_sig_hup(int signo)
{

}

void regist_signal_handler(NodeBase* node)
{
	signal(SIGHUP, on_sig_hup);
}

}
