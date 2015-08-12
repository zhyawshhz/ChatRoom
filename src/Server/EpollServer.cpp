#include "Server/EpollServer.h"
#include "Log/Log.h"

#include <sys/epoll.h>
#include <errno.h>
No1EpollServer*	No1EpollServer::mInstance 					= NULL;

No1EpollServer::No1EpollServer(const No1Config &config)
:
  mPort(config.server_port)
, mCrtReqSize(0)
{
	mBuffer = make_shared<char>(new char[kMaxHandleSize * kOneSockAllSize]);

	if (!init())
	{
		NO1_CLASS_LOG(error, "Failed to init epoll server!!!");
		release();
		return;
	}
}

No1EpollServer::~No1EpollServer()
{
	release();
}

No1EpollServer*
No1EpollServer::get_instance(const No1Config &config)
{
	if (mInstance) return mInstance;
	mInstance = new No1EpollServer(config);
	if (!mInstance)
	{
		NO1_CLASS_LOG(error, "Failed to allocate memory for Class No1EpollServer!!!");
		return NULL;
	}
	return mInstance;
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

	int opt = 1;
	if (setsockopt(mSockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		NO1_CLASS_LOG(error, "Failed to set socket option to reuse addr: " << strerror(errno));
		return false;
	}
	
	if (fcntl(mSockFd, F_SETFL, fcntl(mSockFd, F_GETFD, 0) | O_NONBLOCK) < 0)
	{
		NO1_CLASS_LOG(error, "Failed to set nonblock socket: " << strerror(errno));
		return false;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(mPort);

	if (bind(mSockFd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
	{
		NO1_CLASS_LOG(error, "Failed to bind address: " << strerror(errno));
		return false;
	}

	if (listen(mSockFd, 20) < 0)
	{
		NO1_CLASS_LOG(error, "Failed to listen: " << strerror(errno));
		return false;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = mSockFd;
	if (epoll_ctl(mEpollId, EPOLL_CTL_ADD, mSockFd, &ev) < 0)
	{
		NO1_CLASS_LOG(error, "Failed to add mSockFd to epoll set: " << strerror(errno));
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
	struct epoll_event events[eMaxHandleSize];
	while (true)
	{
		num = epoll_wait(mEpollId, events, static_cast<int>(eMaxHandleSize), 0);
		if (num < 0)
		{
			NO1_CLASS_LOG(error, "epoll wait failed: " << strerror(errno));
			continue;
		}

		for (int i=0; i<num; ++i)
		{
			if (events[i].data.fd = mSockFd)
			{
				socklen_t len;
				struct sockaddr_in client_addr;
				int cfd = accept(mSockFd, (struct sockaddr*)(&client_addr), &len);
				if (cfd < 0)
				{
					NO1_CLASS_LOG(error, "Failed to accept client socket: " << strerror(errno));
					continue;
				}

				NO1_CLASS_LOG(info, "RecvClient: " << inet_ntoa(client_addr.sin_addr) << ", " << client_addr.sin_port);
				
				if (fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFD, 0) | O_NONBLOCK) < 0)
				{
					NO1_CLASS_LOG(error, "Failed to set socket nonblock: " << cfd << " : " << strerror(errno));
					close(cfd);
					continue;
				}
				
				ev.data.fd = cfd;
				ev.events = EPOLLIN | EPOLLET;
				if (epoll_ctl(mEpollId, EPOLL_CTL_ADD, cfd, &ev) < 0)
				{
					NO1_CLASS_LOG(error, "Failed to add socket to epoll events: " << cfd << ", " << strerror(errno));
					close(cfd);
					continue;
				}
				*(int*)(mBuffer + cfd * kOneSockAllSize) = cfd;
				*(int*)(mBuffer + cfd * kOneSockAllSize + kSockFdLen) = 0;
				continue;
			}
			
			int cfd = events[i].data.fd;
			int sock_start_len = cfd * kOneSockAllSize;
			while (true)
			{
				int bodylen = *(int*)(mBuffer + sock_start_len + kSockFdLen); 
				int nread = read(cfd, mBuffer + sock_start_len + kOneSockHeaderSize + bodylen, kPerReadSize);
				if (nread == 0)
				{
					NO1_CLASS_LOG(info, "Client close: " << cfd);
					if (epoll_ctl(mEpollId, EPOLL_CTL_DEL, cfd, &ev) < 0)
					{
						NO1_CLASS_LOG(error, "Failed to delete socket to epoll events: " << cfd << ", " << strerror(errno));
					}
					close(cfd);
					break;
				}
				if (nread < 0)
				{
					NO1_CLASS_LOG(error, "Failed to read: " << cfd << ", " << strerror(errno));
					close(cfd);
					break;
				}
				
				mLock->lock();
				*(int*)(mBuffer + cfd * kOneSockAllSize + kSockFdLen) = bodylen + nread;
				mCondVar->signal();
				mLock->unlock();
			}
		}
	}
}

void
No1EpollServer::stop()
{
	if (m_instance) delete m_instance;
}
