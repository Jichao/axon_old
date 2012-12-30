
#include "connect.h"
#include "os_misc.h"

namespace axon {

Connect::Connect() : next_(NULL), mgr_(NULL)
{
	rbuf_ = wbuf_ = NULL;
	ident_ = 0;
	reset();
}

Connect::~Connect()
{
	delete rbuf_;
	delete wbuf_;
}

void Connect::init(IConnectHandler *m, int fd, int rsize, int wsize)
{
	RT_ASSERT(m != NULL && fd >= 0 && rsize > 0 && wsize > 0);
	mgr_ = m;
	fd_ = fd;
	if (rbuf_) {
		rbuf_->reset(rsize);
	} else {
		rbuf_ = new buffer_t(rsize);
	}

	if (wbuf_) {
		wbuf_->reset(wsize);
	} else {
		wbuf_ = new buffer_t(wsize);
	}
}

void Connect::reset()
{
	fd_ = -1;
	index_ = -1;
	peer_port_ = 0;	
	ev_handle_ = 0;
	peer_ip_ = string_t("0.0.0.0");
	if (rbuf_) rbuf_->clear();
	if (wbuf_) wbuf_->clear();
	status_ = 0;
}

void Connect::close()
{
	if (fd_ >= 0) ::close(fd_);
	reset();
}

void Connect::on_ev_read(int fd)
{
	if (mgr_ == NULL) return;
	mgr_->on_read(this);
}

void Connect::on_ev_write(int fd)
{
	if (mgr_ == NULL) return;
	mgr_->on_write(this);

}

//Listener===================================

Listener::Listener() : mgr_(NULL)
{
	listening_ = 0;
	fd_ = -1;
	port_ = backlog_ = 0;
	ident_ = 0;
}

Listener::~Listener() 
{

}

int Listener::init(IListenHandler* mgr, uint16_t port, int backlog)
{
	struct sockaddr_in sin;
	int optval = 1;
	int fd;
	RT_ASSERT(mgr != NULL);
   	RT_ASSERT(port > 0 && backlog > 0);

	//init fd
	bzero(&sin, sizeof(sin));
	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		debug_log("[Listener] cannot create listener socket");
		return -1;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
				(char *) &optval, sizeof(optval)) == -1)
	{
		debug_log("[Listener] cannot set REUSEADDR for listener");
		return -1;
	}
	
	fd_ = fd;	
	mgr_ = mgr;
	port_ = port;
	backlog_ = backlog;

	return 0;
}

void Listener::on_ev_read(int fd)
{
	if (mgr_ == NULL || !listening_) return;
	mgr_->on_listen_read(this);
}

void Listener::on_ev_write(int fd)
{
	
}

void Listener::close()
{
	if (fd_ >= 0) ::close(fd_);
	fd_ = -1;
}

int Listener::listen()
{
	struct sockaddr_in sin;
	
	//not init correctly
	if (port_ == 0 || fd_ < 0 || backlog_ < 1) return -1;
	if (mgr_ == NULL) return -1;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((unsigned short)port_);
	if (::bind(fd_, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		debug_log("[Listener] cannot bind listener. port=%d", port_);
		return -1;
	}
	ax_set_nonblock(fd_);
	::listen(fd_, backlog_);
	debug_log("[Listener] start listen. port= %d", port_);	
	listening_ = 1;
	return 0;
}


} //namespace
