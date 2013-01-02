
#include "connect_worker.h"

ConnectWorker::ConnectWorker(EvPoller* poller)
{
	poller_ = poller;
}

ConnectWorker::~ConnectWorker()
{

}

Connect* ConnectWorker::new_connect(int fd, int hid)
{
	int vfd = container_->alloc_connect(fd);
	
}

void ConnectWorker::on_read(Connect* conn)
{

}

void ConnectWorker::on_write(Connect* conn)
{

}

