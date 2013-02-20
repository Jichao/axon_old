//process node communication manager
//

#ifndef _AX_NODE_COMM_H_
#define _AX_NODE_COMM_H_

namespace axon {

const int MAX_NODE_NUM = 255;

class NodeComm
{
public:
	NodeComm();
	~NodeComm();
protected:
	Connect* node_conn; 

};



} //namespace

#endif
