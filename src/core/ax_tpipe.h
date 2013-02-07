//pipe between threads using socketpair
// 0 for main thread , 1 for worker thread
// thread 0 is responsible for tpipe create and destruction


#ifndef _AX_TPIPE_H_
#define _AX_TPIPE_H_

#include "ax_queue.h"
#include "ax_buffer.h"
#include "fd_poller.h"
#include "ax_proto_msg.h"

namespace axon {

//callback function.
//don't reference pointer of "data" outside the callback function.
//it will be invalid later. copy them if needed.
typedef void (*pipe_cbfunc) (void* pobj, msg_header_t header, char* data);

class tpipe_t : IFdEventReactor
{
public:
	tpipe_t();
	~tpipe_t();
	void init(int side, EvPoller* poller, pipe_cbfunc cb_read, void* cbobj);
	int get_fd0() { return pinfo_[0].fd_; }  //usually a for main thread
	int get_fd1() { return pinfo_[1].fd_; }  //b for worker thread

	//write to tpipe
	int write(int side, int wrapper_type, proto_msg_t* wrapper, proto_msg_t* pkt);

	void try_write(int side);
	virtual void on_ev_read(int fd);
	virtual void on_ev_write(int fd);

private:
	//info for one side
	typedef struct tpipe_fd_info_s {
		int fd_;
		buffer_t *wbuf_;
		buffer_t *rbuf_;
		pipe_cbfunc cb_read_;
		EvPoller *poller_;
		void* cbobj_;
		//split to seperate cache-line, 
		//to avoid cache-line shared by different threads
		//assume the CPU is x86-64. cache-line is 64 byte
		char _dummy[64];  
	} tpipe_fd_info_t;

	//disable copy constructor
	tpipe_t(const tpipe_t& rhs);
	const tpipe_t &operator = (const tpipe_t&);
	
private:
	static const int init_buf_size = 32000;  //32K init cache
	tpipe_fd_info_t pinfo_[2];
};


}

#endif
