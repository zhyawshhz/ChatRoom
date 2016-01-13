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
{

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
	struct rlimit rt;
	rt.rlim_max = rt.rlim_cur = kMaxHandleSize;
	if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
	{
		GLOBAL_LOG_SEV(error, "Failed to set process max file!!!");
		return false;
	}

	m_epoll_id = epoll_create(kMaxHandleSize);
	if (m_epoll_id < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to create epoll: " << strerror(errno));
		return false;
	}

	m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock_fd < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to create socket: " << strerror(errno));
		return false;
	}

	int opt = 1;
	if (setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to set socket option to reuse addr: " << strerror(errno));
		return false;
	}
	
	if (fcntl(m_sock_fd, F_SETFL, fcntl(m_sock_fd, F_GETFD, 0) | O_NONBLOCK) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to set nonblock socket: " << strerror(errno));
		return false;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(m_port);

	if (bind(m_sock_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to bind address: " << strerror(errno));
		return false;
	}

	if (listen(m_sock_fd, 20) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to listen: " << strerror(errno));
		return false;
	}

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

void
No1EpollServer::release()
{
	::close(mEpollId);
	::close(mSockFd);
}

void
No1EpollServer::start()
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
			if (events[i].data.fd = m_sock_fd)
			{
				socklen_t len;
				struct sockaddr_in client_addr;
				int cfd = accept(m_sock_fd, (struct sockaddr*)(&client_addr), &len);
				if (cfd < 0)
				{
					GLOBAL_LOG_SEV(error, "Failed to accept client socket: " << strerror(errno));
					continue;
				}

				GLOBAL_LOG_SEV(info, "RecvClient: " << inet_ntoa(client_addr.sin_addr) << ", " << client_addr.sin_port);
				
				if (fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFD, 0) | O_NONBLOCK) < 0)
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
				*(int*)(m_buffer + cfd * kOneSockAllSize) = cfd;
				*(int*)(m_buffer + cfd * kOneSockAllSize + kSockFdLen) = 0;
				continue;
			}
			
			int cfd = events[i].data.fd;
			int sock_start_len = cfd * kOneSockAllSize;
			while (true)
			{
				int bodylen = *(int*)(m_buffer + sock_start_len + kSockFdLen);
				int nread = read(cfd, m_buffer + sock_start_len + kOneSockHeaderSize + bodylen, kPerReadSize);
				if (nread == 0)
				{
					GLOBAL_LOG_SEV(info, "Client close: " << cfd);
					if (epoll_ctl(m_epoll_id, EPOLL_CTL_DEL, cfd, &ev) < 0)
					{
						GLOBAL_LOG_SEV(error, "Failed to delete socket to epoll events: " << cfd << ", " << strerror(errno));
					}
					close(cfd);
					break;
				}
				if (nread < 0)
				{
					GLOBAL_LOG_SEV(error, "Failed to read: " << cfd << ", " << strerror(errno));
					close(cfd);
					break;
				}
				
			//	mLock->lock();
				*(int*)(m_buffer + cfd * kOneSockAllSize + kSockFdLen) = bodylen + nread;
			//	mCondVar->signal();
			//	mLock->unlock();
			}
		}
	}
}

void
No1EpollServer::stop()
{

}
