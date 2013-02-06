
#include "ax_tpipe.h"
#include "stdheader.h"
#include "ax_debug.h"
#include "os_misc.h"
#include <proto/node_msg_pb.h>
#include <proto/cgate_msg_pb.h>
#include <cstddef>

using namespace xpb;

namespace axon {

tpipe_t::tpipe_t()
{
	//create socket pair
	int sv[2];
	int ret;
    ret	= socketpair (AF_UNIX, SOCK_STREAM, 0, sv);
	//create before thread create. must be ok. 
	RT_ASSERT(ret == 0);
	pinfo_[0].fd_ = sv[0];
	pinfo_[1].fd_ = sv[1];
	ax_set_nonblock(sv[0]);
	ax_set_nonblock(sv[1]);
	for (int i=0; i<2; i++) {
		pinfo_[i].wbuf_ = new buffer_t(init_buf_size);
		pinfo_[i].rbuf_ = new buffer_t(init_buf_size);
		pinfo_[i].cb_read_ = NULL;
		pinfo_[i].poller_ = NULL;

	}
}

tpipe_t::~tpipe_t()
{
	for(int i=0; i<2; i++) {
		delete pinfo_[i].wbuf_;
		delete pinfo_[i].rbuf_;
		if (pinfo_[i].poller_)  {
			pinfo_[i].poller_->close_fd(pinfo_[i].fd_);
		}
		::close(pinfo_[i].fd_);
	}
}

void tpipe_t::init(int side, EvPoller* poller, pipe_cbfunc cb_read, void* cbobj)
{
	RT_ASSERT(poller != NULL);
	RT_ASSERT(side == 0 || side == 1);
	pinfo_[side].poller_ = poller;
	pinfo_[side].cbobj_ = cbobj;
	pinfo_[side].cb_read_ = cb_read;
	poller->add_fd(pinfo_[side].fd_, this);
}

//Warning: two thread share this callback function without any lock
//don't share any global variable in two branch
void tpipe_t::on_ev_read(int fd)
{
	int side;
	int nbytes;
	int remain;
	int ret;
	buffer_t* buf;
	pb_wrapper wrapper;
	if (fd == pinfo_[0].fd_) {
		side = 0;
	} else if (fd == pinfo_[0].fd_) {
		side = 1;
	} else {
		//error
		return;
	}
	buf = pinfo_[side].rbuf_;
	buf->prepare( MAX_PROTO_LEN );
	nbytes = ::read(pinfo_[side].fd_, buf->tail(), MAX_PROTO_LEN);
	if (nbytes < 0) {
		//error, ignore right now. handle it later
		if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) return;
		pinfo_[side].poller_->wait_close(pinfo_[side].fd_);
		return;
	}
	buf->flush_push(nbytes);	
	if (pinfo_[side].cb_read_ == NULL) {
		buf->pop(nbytes);
		return;
	}
	remain = nbytes;
	while( (ret = wrapper.unpack(buf->data(), remain)) > 0) {
		if (wrapper.pl.pl_len > remain) return;  //wait more data
		buf->pop(ret);
		ret = pinfo_[side].cb_read_(pinfo_[side].cbobj_, &wrapper, remain - ret);
		if (ret < 0) {
			//packet error
			buf->pop(wrapper.pl.pl_len);	
			return;
		}
		buf->pop(wrapper.pl.pl_len);
		if (wrapper.pl.pl_len != ret) {
			//packet error
			return;
		}
		remain -= ret;
	}
}

//try to flush the write buffer
void tpipe_t::try_write(int side)
{
	int nbytes;
	int fd;
	buffer_t *buf;
	RT_ASSERT(side == 0 || side == 1);
   	fd = pinfo_[side].fd_;
	buf = pinfo_[side].wbuf_;
	nbytes = ::write(fd, buf, buf->len());	
	if (nbytes < 0) {
		if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) return;
		pinfo_[side].poller_->wait_close(pinfo_[side].fd_);
		return;
	}
	if (buf->len() < 1) {
		//clear write buffer. del event
		pinfo_[side].poller_->del_event(fd, EV_WRITE);
	}
}

//Warning: two thread share this callback function without any lock
//don't share any global variable in two branch
void tpipe_t::on_ev_write(int fd)
{
	int side;
	if (fd == pinfo_[0].fd_) {
		side = 0;
	} else if (fd == pinfo_[1].fd_) {
		side = 1;
	} else {
		//error
		return;
	}
	try_write(side);
}

//write
int tpipe_t::write(int side, proto_msg_t* p)
{
	int fd;
	int ret;
	int len;
	buffer_t *buf;
	RT_ASSERT(side == 0 || side == 1);
	fd = pinfo_[side].fd_;
	buf = pinfo_[side].wbuf_;
	if (pinfo_[side].poller_ == NULL) {
		//not init yet
		return -1;
	}
	len = p->cal_size();
	buf->prepare( len + 8 );
	ret = p->pack(buf->tail(), len + 8);
	if (ret < 0) return -1;
	buf->flush_push(ret);
	try_write(side);
	
	return 0;
}

} //namespace
