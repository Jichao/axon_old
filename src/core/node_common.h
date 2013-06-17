
#ifndef _AX_NODE_COMMON_H
#define _AX_NODE_COMMON_H

#include "ax_core.h"
#include "node_base.h"

namespace axon {

const static int INTERNAL_HOSTNUM = 89;
const static int MAX_NODE_CNT = 100;  //max node count in one cluster

class NodeBase;

struct CommonArgHandler
{
	int operator() (int argc, char** argv, NodeBase* node) const {
		int err = AX_RET_OK;
		char ch;
		while ((ch = getopt(argc, argv, "c:")) != -1) {
			switch(ch) {
			case 'c':
				err = node->load_node_config(optarg, 0);
				break;
			default:
				break;
			}
		}
		return err;
	}
};

void regist_signal_handler(NodeBase* node);


template <class Node_T, class ArgsHandler>
int _ax_node_main(int argc, char** argv)
{
	int ret ;
	Node_T main_node;
	ret = ArgsHandler(argc, argv, (NodeBase*)&main_node);
	if (ret == AX_RET_ERROR) {
		debug_log("parse argv error.");
		return -1;
	}
	regist_signal_handler((NodeBase*)&main_node);

	return main_node.run();
}

}

#endif 
