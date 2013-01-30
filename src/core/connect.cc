//TCP connect
//
#include "connect.h"
#include "os_misc.h"

namespace axon {

Connect::Connect() : mgr_(NULL), next_(NULL)
{
	rbuf_ = wbuf_ = NULL;
	fd_ = 0;
	reset();
}

Connect::~Connect()
{
	delete rbuf_;
	delete wbuf_;
}

// Init connection 
//param: 
//    m      connection manager
int Connect::init(IConnectHandler *m, int fd, int rsize, int wsize)
{
	RT_ASSERT(m != NULL);
	RT_ASSERT(fd >= 0 && rsize > 0 && wsize > 0);
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

	return AX_RET_OK;
}

void Connect::reset()
{
	fd_ = -1;
	index_ = -1;
	hid_ = -1;
	peer_port_ = 0;	
	ev_handle_ = 0;
	peer_ip_ = string_t("0.0.0.0");
	if (rbuf_) rbuf_->clear();
	if (wbuf_) wbuf_->clear();
	status_ = 0;
}

//close socket and reset
void Connect::close()
{
	if (fd_ >= 0) ::close(fd_);
	reset();
}

//read bytes
int Connect::read()
{
	int nbytes;
	RT_ASSERT(rbuf_ != NULL);
	//rbuf at least 2KB
	rbuf_->prepare(2048);
	nbytes = ::read(fd_, rbuf_->tail(), 2048);
	if (nbytes <= 0) return nbytes;
	rbuf_->flush_push(nbytes);
	return nbytes;
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

//ConnectContainer======================
//
//param: n   max connection capacity
ConnectContainer::ConnectContainer(uint32_t n, IConnectHandler* mgr, uint32_t rbuf_size, uint32_t wbuf_size) :
	 connects_(NULL), head_(NULL)
{
	int i;
	connects_ = new Connect[n];
	RT_ASSERT(connects_ != NULL);
	free_stack_ = new int[n];
	RT_ASSERT(free_stack_ != NULL);
	count_ = 0;
	capacity_ = n;
	stack_top_ = 0;
	for(i=0; i<(int)n; i++) {
		free_stack_[i] = i;
	}
	rsize_ = rbuf_size;
	wsize_ = wbuf_size;
	mgr_ = mgr;
}

ConnectContainer::~ConnectContainer()
{
	delete[] connects_;
	delete[] free_stack_;
}

//return a virtual fd to identify connect (actually a array index)
int ConnectContainer::alloc_connect(int sockfd)
{
	int vfd;
	Connect* c;
	if (count_ >= capacity_) return AX_RET_FULL;
	RT_ASSERT(mgr_ != NULL && connects_ != NULL);
	
	vfd = free_stack_[stack_top_];
	RT_ASSERT(vfd >= 0 && vfd < capacity_);

	c = &connects_[vfd];
	c->init(mgr_, sockfd, rsize_, wsize_);
	c->next_ = head_;
	head_ = c;

	++stack_top_;
	++count_;
	return vfd;	
}

void ConnectContainer::free_connect(int vfd)
{
	Connect* c;
	RT_ASSERT(stack_top_ >= 0 && count_ >= 0);
	RT_ASSERT(vfd >= 0 && vfd < capacity_ && connects_ != NULL);

	c = &connects_[vfd];
	c->reset();
	--stack_top_;
	--count_;
	free_stack_[stack_top_] = vfd;
}

Connect* ConnectContainer::get_connect(int vfd)
{
	RT_ASSERT(connects_ != NULL);
	if (vfd > capacity_ - 1 || vfd < 0) return NULL;
	return &connects_[vfd];
}



//Listener===================================

Listener::Listener() : mgr_(NULL)
{
	listening_ = 0;
	fd_ = -1;
	port_ = backlog_ = 0;
	ident_ = 0;
	ev_handle_ = 0;
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
		return AX_RET_ERROR;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
				(char *) &optval, sizeof(optval)) == -1)
	{
		debug_log("[Listener] cannot set REUSEADDR for listener");
		return AX_RET_ERROR;
	}
	
	fd_ = fd;	
	mgr_ = mgr;
	port_ = port;
	backlog_ = backlog;

	return AX_RET_OK;
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

int Listener::listen(EvPoller *poller)
{
	struct sockaddr_in sin;
	
	//not init correctly
	if (port_ == 0 || fd_ < 0 || backlog_ < 1) return AX_RET_ERROR;
	if (mgr_ == NULL) return AX_RET_ERROR;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((unsigned short)port_);
	if (::bind(fd_, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		return AX_RET_ERROR;
	}
	ax_set_nonblock(fd_);
	::listen(fd_, backlog_);
	listening_ = 1;
	ev_handle_ = poller->add_fd(fd_, this);
	poller->add_event(fd_, ev_handle_, EV_READ);
	return AX_RET_OK;
}


} //namespace
