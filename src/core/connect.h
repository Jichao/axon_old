//network connection and TCP socket event wrapper
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

class ConnectContainer;

//socket connection
class Connect : public IFdEventReactor
{
public:
	friend class ConnectContainer;
	Connect();
	~Connect();
	int init(IConnectHandler* mgr, int fd, int rsize, int wsize);
	void close();
	void reset();
	int read();
	bool is_active() { return fd_ > 0 && mgr_ != NULL && peer_port_ > 0;}

	virtual void on_ev_read(int fd);
	virtual void on_ev_write(int fd);

//direct manipulated by connection manager
public:	
	int fd_;
	//addition tag variable
	int index_;     //index in container
	int status_;    //connection state machine variable
	int hid_;     //identify unique connection in mgr_

	string_t peer_ip_;
	uint16_t peer_port_;   //peer's port

	ev_handle_t ev_handle_;	
	buffer_t *rbuf_;
	buffer_t *wbuf_;

protected:
	IConnectHandler *mgr_;   //manage class to handler socket process
	Connect* next_;        //used as linklist
};

//a bundle of connections
class ConnectContainer
{
public:
	ConnectContainer(uint32_t n, IConnectHandler* mgr, uint32_t rbuf_size, uint32_t wbuf_size);
	~ConnectContainer();

	int alloc_connect(int sockfd);
	void free_connect(int vfd);
	Connect* get_connect(int vfd);
	int count() { return count_; }
	int capacity() { return capacity_; }

private:
	int max_ident_;
	uint32_t rsize_ ;   //rbuffer size
	uint32_t wsize_ ;   //wbuffer size
	int count_;
	int capacity_;
	int max_vfd_;
	Connect *connects_;  //connection array
	Connect *head_;     //connect iterator begin
	int *free_stack_;
	int stack_top_;
	IConnectHandler *mgr_;
};


class Listener : public IFdEventReactor
{
public:
	Listener();
	~Listener();

	int init(IListenHandler* mgr, uint16_t port, int backlog);
	int listen(EvPoller* poller);
	void close();
	virtual void on_ev_read(int fd);
	virtual void on_ev_write(int fd);
	int get_port() { return port_; }

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
