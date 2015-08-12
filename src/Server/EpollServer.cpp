#include "Server/EpollServer.h"
#include "Log/Log.h"

#include <sys/epoll.h>
#include <errno.h>
No1EpollServer*	No1EpollServer::m_instance 					= NULL;

No1EpollServer::No1EpollServer(const No1Config &config)
:
m_port(config.server_port)
{
	if (!init())
	{
		NO1_CLASS_LOG(error, "Failed to init epoll server!!!");
		return;
	}
}

No1EpollServer::~No1EpollServer()
{
	::close(mEpollId);
}

No1EpollServer*
No1EpollServer::get_instance()
{
	if (m_instance) return m_instance;
	m_instance = new No1EpollServer();
	if (!m_instance)
	{
		NO1_CLASS_LOG(error, "Failed to allocate memory for Class No1EpollServer!!!");
		return NULL;
	}
	return m_instance;
}

bool
No1EpollServer::init()
{
	// set max open file per process
	struct rlimit rt;
	rt.rlim_max = rt.rlim_cur = eMaxHandleSize;
	if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
	{
		NO1_CLASS_LOG(error, "Failed to set process max file!!!");
		return false;
	}

	mEpollId = epoll_create(eMaxHandleSize);
	if (mEpollId < 0)
	{
		NO1_CLASS_LOG(error, "Failed to create epoll: " << strerror(errno));
		return false;
	}

	mSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (mSockFd < 0)
	{
		NO1_CLASS_LOG(error, "Failed to create socket: " << strerror(errno));
		rerturn false;
	}
}

void
No1EpollServer::start()
{

}

void
No1EpollServer::stop()
{
	if (m_instance) delete m_instance;
}
