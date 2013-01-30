
#include "ax_tpipe.h"
#include "stdheader.h"
#include "ax_debug.h"
#include "os_misc.h"
#include <cstddef>

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
		pinfo_[i].ev_h_ = 0;
		pinfo_[i].poller_ = NULL;

	}
}

tpipe_t::~tpipe_t()
{
	for(int i=0; i<2; i++) {
		delete pinfo_[i].wbuf_;
		delete pinfo_[i].rbuf_;
		if (pinfo_[i].poller_)  {
			pinfo_[i].poller_->rm_fd(pinfo_[i].fd_);
		}
		::close(pinfo_[i].fd_);
	}
}

void tpipe_t::init(int side, EvPoller* poller, pipe_cbfunc cb_read, void* cbobj)
{
	RT_ASSERT(poller != NULL);
	RT_ASSERT(side == 0 || side == 1);
	pinfo_[side].poller_ = poller;
	pinfo_[side].ev_h_ = poller->add_fd(pinfo_[side].fd_, this);
	pinfo_[side].cbobj_ = cbobj;
	pinfo_[side].cb_read_ = cb_read;
	poller->add_event(pinfo_[side].fd_, pinfo_[side].ev_h_, EV_READ);
}

//Warning: two thread share this callback function without any lock
//don't share any global variable in two branch
void tpipe_t::on_ev_read(int fd)
{
	int side;
	uint32_t total_len;
	int nbytes;
	buffer_t* buf;
	var_msg_t *pdata;
	if (fd == pinfo_[0].fd_) {
		side = 0;
	} else if (fd == pinfo_[0].fd_) {
		side = 1;
	} else {
		//error
		return;
	}
	buf = pinfo_[side].rbuf_;
	buf->prepare(8182);
	nbytes = ::read(pinfo_[side].fd_, buf->tail(), 8182);
	if (nbytes < 0) {
		//error, ignore right now. handle it later
		return;
	}
	buf->flush_push(nbytes);	
	while(buf->len() >= sizeof(var_msg_t) ) {
		pdata = (var_msg_t*)buf->data();
		total_len = pdata->length + VAR_MSG_HLEN;
		if (total_len > buf->len()) {
			//require more income data
			break;
		}
		if (pinfo_[side].cb_read_) {
			pinfo_[side].cb_read_(pinfo_[side].cbobj_, pdata);
		}
		//don't reference pdata elsewhere
		buf->pop(total_len);
	}
}

//Warning: two thread share this callback function without any lock
//don't share any global variable in two branch
void tpipe_t::on_ev_write(int fd)
{
	int side;
	int nbytes;
	buffer_t* buf;
	if (fd == pinfo_[0].fd_) {
		side = 0;
	} else if (fd == pinfo_[1].fd_) {
		side = 1;
	} else {
		//error
		return;
	}
	buf = pinfo_[side].wbuf_;
	nbytes = ::write(fd, buf, buf->len());	
	if (nbytes < 0) {
		//error
		return;
	}
	if (buf->len() < 1) {
		//clear write buffer. del event
		pinfo_[side].poller_->del_event(fd, pinfo_[side].ev_h_, EV_WRITE);
	}
}

//write
int tpipe_t::write(int side, int payload_len, int type, char* payload)
{
	int fd;
	int nbytes;
	int remain;
	var_msg_t msg;
	RT_ASSERT(side == 0 || side == 1);
	fd = pinfo_[side].fd_;
	if (pinfo_[side].poller_ == NULL) {
		//not init yet
		return -1;
	}
	msg.length = payload_len;
	msg.type = type;
	nbytes = ::write(fd, &msg, VAR_MSG_HLEN);
	if (nbytes < 0) return -1;

	if (nbytes < VAR_MSG_HLEN) {
		remain = VAR_MSG_HLEN - nbytes;
		pinfo_[side].wbuf_->push(((char*)&msg) + nbytes, remain);
		pinfo_[side].wbuf_->push(payload, payload_len);	
		return 0;	
	}
	nbytes = ::write(fd, payload, payload_len);
	if (nbytes < 0) return -1;

	if (nbytes < payload_len) {
		remain = payload_len - nbytes;
		pinfo_[side].wbuf_->push(payload + nbytes, remain);
	}
	return 1;
}

} //namespace
