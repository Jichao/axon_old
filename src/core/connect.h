//network connection and socket event wrapper
//

#ifndef _AX_CONNECT_H_
#define _AX_CONNECT_H_

#include "stdheader.h"
#include "ax_string.h"
#include "fd_poller.h"
#include "ax_buffer.h"

namespace axon {

class Connect;
class Listener;

//connect manager interface (connection owner)
class IConnectHandler
{
public:
	virtual void on_read(Connect* conn) = 0;
	virtual void on_write(Connect* conn) = 0;
};

class IListenHandler
{
public:
	virtual void on_listen_read(Listener* ls) = 0;
};

class Connect : public IFdEventReactor
{
public:
	Connect();
	~Connect();
	void init(IConnectHandler* mgr, int fd, int rsize, int wsize);
	void close();
	void reset();
	bool is_active() { return fd_ > 0 && mgr_ != NULL && peer_port_ > 0;}

	virtual void on_ev_read(int fd);
	virtual void on_ev_write(int fd);

//direct manipulated by connection manager
public:	
	int fd_;
	ev_handle_t ev_handle_;	
	int index_;
	int status_;
	int ident_;     //identify fd type to decide action in mgr_

	string_t peer_ip_;
	uint16_t peer_port_;   //peer's port

	Connect* next_;
	IConnectHandler *mgr_;
	buffer_t *rbuf_;
	buffer_t *wbuf_;
};

class Listener : public IFdEventReactor
{
public:
	Listener();
	~Listener();

	int init(IListenHandler* mgr, uint16_t port, int backlog);
	int listen();
	void close();
	virtual void on_ev_read(int fd);
	virtual void on_ev_write(int fd);

//direct manipulated by connection manager
public:
	ev_handle_t ev_handle_;
	int backlog_;
	int listening_;
	int fd_;	
	uint16_t port_;
	int ident_;     //identify fd type to decide action in mgr_
	IListenHandler *mgr_;
};

} //namespace

#endif
