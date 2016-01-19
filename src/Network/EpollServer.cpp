#include "Network/EpollServer.h"

#include "Log/Log.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

No1EpollServer*	No1EpollServer::m_instance 					= NULL;

No1EpollServer::No1EpollServer()
:
m_port(-1)
, m_stop(true)
{
	m_sessions.clear();
	m_port = No1Config::get_instance()->get_server_port();
	if (!init())
	{
		GLOBAL_LOG_SEV(error, "Failed to init epoll server!!!");
		release();
		return;
	}
}

No1EpollServer::~No1EpollServer()
{
	release();
	if (m_instance) delete m_instance;
}

No1EpollServer*
No1EpollServer::get_instance()
{
	if (m_instance) return m_instance;
	m_instance = new No1EpollServer();
	return m_instance;
}

bool
No1EpollServer::init()
{
	// set max open file per process
	if (!set_max_open_file(kMaxHandleSize))
	{
		GLOBAL_LOG_SEV(error, "Failed to set process max file!!!");
		return false;
	}

	// create epoll
	m_epoll_id = epoll_create(kMaxHandleSize);
	if (m_epoll_id < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to create epoll: " << strerror(errno));
		return false;
	}

	// create listen socket
	m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock_fd < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to create socket: " << strerror(errno));
		return false;
	}

	// set reuse socket address
	int opt = 1;
	if (!set_reuse_addr(m_sock_fd))
	{
		GLOBAL_LOG_SEV(error, "Failed to set socket option to reuse addr: " << strerror(errno));
		return false;
	}
	
	// set socket noblock
	if (!set_noblock(m_sock_fd))
	{
		GLOBAL_LOG_SEV(error, "Failed to set nonblock socket: " << strerror(errno));
		return false;
	}

	// set socket listen port
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(m_port);

	if (bind(m_sock_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to bind address: " << strerror(errno));
		return false;
	}

	// listen
	if (listen(m_sock_fd, 20) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to listen: " << strerror(errno));
		return false;
	}

	// add listen socket to epoll events
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = m_sock_fd;
	if (epoll_ctl(m_epoll_id, EPOLL_CTL_ADD, m_sock_fd, &ev) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to add mSockFd to epoll set: " << strerror(errno));
		return false;
	}
	
	return true;
}


bool
No1EpollServer::init_thrds()
{
	int thrd_num = No1Config::get_instance()->get_handle_thrd_num();
	for (int i=0; i<thrd_num+1; ++i)
	{
		No1Thread t(i, this);
		m_thrds.push_back(t);
	}
	return true;
}


void
No1EpollServer::release()
{
	::close(m_epoll_id);
	::close(m_sock_fd);
}

bool
No1EpollServer::threadFunc(const int id)
{
	if (id == 0)
	{
		return proc_recv_client();
	}

	return proc_msg();
}


bool
No1EpollServer::proc_msg()
{
	return true;
}


bool
No1EpollServer::proc_recv_client()
{
	int num;
	struct epoll_event ev;
	struct epoll_event events[kMaxHandleSize];
	while (true)
	{
		num = epoll_wait(m_epoll_id, events, static_cast<int>(kMaxHandleSize), 0);
		if (num < 0)
		{
			GLOBAL_LOG_SEV(error, "epoll wait failed: " << strerror(errno));
			continue;
		}

		for (int i=0; i<num; ++i)
		{
			if (events[i].data.fd == m_sock_fd)
			{// accept new client
				socklen_t len;
				struct sockaddr_in client_addr;
				int cfd = accept(m_sock_fd, (struct sockaddr*)(&client_addr), &len);
				if (cfd < 0)
				{
					GLOBAL_LOG_SEV(error, "Failed to accept client socket: " << strerror(errno));
					continue;
				}

				GLOBAL_LOG_SEV(info, "RecvClient: " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port));
				
				if (!set_noblock(cfd))
				{
					GLOBAL_LOG_SEV(error, "Failed to set socket nonblock: " << cfd << " : " << strerror(errno));
					close(cfd);
					continue;
				}
				
				ev.data.fd = cfd;
				ev.events = EPOLLIN | EPOLLET;
				if (epoll_ctl(m_epoll_id, EPOLL_CTL_ADD, cfd, &ev) < 0)
				{
					GLOBAL_LOG_SEV(error, "Failed to add socket to epoll events: " << cfd << ", " << strerror(errno));
					close(cfd);
					continue;
				}

				m_lock->lock();
				m_sessions[cfd] = boost::make_shared<No1ServerSession>(cfd, client_addr.sin_addr, client_addr.sin_port);
				m_lock->unlock();

				continue;
			}
			
			int cfd = events[i].data.fd;

			m_lock->lock();
			auto itr = m_sessions.find(cfd);
			if (itr == m_sessions.end())
			{
				GLOBAL_LOG_SEV(error, "Not Found In Cache: " << cfd);
				m_lock->unlock();
				continue;
			}

			itr->second->proc();
			m_lock->unlock();
		}
	}
	return false;
}

void
No1EpollServer::stop()
{
	m_stop = true;
}


void
No1EpollServer::start()
{
	if (!init_thrds())
	{
		GLOBAL_LOG_SEV(error, "Failed to init thread!!!");
		release();
		return;
	}
}
