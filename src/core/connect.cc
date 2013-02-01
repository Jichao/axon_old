//TCP connection and connect container
// no thread safe guaranteed, never pass it between threads

#include "connect.h"
#include "os_misc.h"

namespace axon {

Connect::Connect() : mgr_(NULL), next_(NULL), prev_(NULL)
{
	rbuf_ = wbuf_ = NULL;
	fd_ = -1;
	index_ = -1;
	hid_ = 0;
	status_ = CONN_EMPTY;
	ident_ = 0;
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
	peer_port_ = 0;
	peer_ip_ = 0;
	status_ = CONN_EMPTY;
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

//read bytes
int Connect::read()
{
	int nbytes;
	RT_ASSERT(rbuf_ != NULL);
	if (status_ != CONN_ACTIVE) {
		return -1;
	}

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
// prealloc enough connection object, never free and realloc in server runtime
//
//param: n   max connection capacity
//
ConnectContainer::ConnectContainer(uint32_t n, IConnectHandler* mgr, uint32_t rbuf_size, uint32_t wbuf_size) :
	 connects_(NULL), head_(NULL)
{
	int i;
	connects_ = new Connect[n];
	free_stack_ = new int[n];
	RT_ASSERT(connects_ != NULL && free_stack_ != NULL);
	count_ = 0;
	capacity_ = n;
	stack_top_ = 0;
	for(i=0; i<(int)n; i++) {
		connects_[i].index_ = i;
		free_stack_[i] = i;
	}
	rsize_ = rbuf_size;
	wsize_ = wbuf_size;
	mgr_ = mgr;

}

ConnectContainer::~ConnectContainer()
{
	delete[] connects_;
}

int ConnectContainer::alloc_connect(int sockfd)
{
	int vfd;
	Connect* c;
	if (count_ >= capacity_) return AX_RET_FULL;
	RT_ASSERT(mgr_ != NULL);
	
	vfd = free_stack_[stack_top_];
	RT_ASSERT(vfd >= 0 && vfd < capacity_);

	c = &connects_[vfd];
	if (c->status_ != CONN_EMPTY) return AX_RET_ERROR;
	c->init(mgr_, sockfd, rsize_, wsize_);
	c->next_ = head_;
	c->prev_ = NULL;
	if (head_) head_->prev_ = c;
	head_ = c;

	++stack_top_;
	++count_;
	return vfd;	
}

void ConnectContainer::free_connect(int vfd)
{
	Connect* c;
	Connect* prev;
	RT_ASSERT(stack_top_ > 0 && count_ > 0);
	RT_ASSERT(vfd >= 0 && vfd < capacity_);

	c = &connects_[vfd];
	c->status_ = CONN_EMPTY;
	prev = c->prev_;
	if (prev) prev->next_ = c->next_;
	if (c->next_) c->next_->prev_ = c->prev_;

	--count_;
	--stack_top_;
	free_stack_[stack_top_] = vfd;
}

Connect* ConnectContainer::get_connect(int vfd)
{
	if (vfd >= capacity_ || vfd < 0) return NULL;
	return &connects_[vfd];
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

int Listener::listen(EvPoller *poller)
{
	struct sockaddr_in sin;
	int ret;

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
	ret = poller->add_fd(fd_, this);
	return ret;
}


} //namespace
