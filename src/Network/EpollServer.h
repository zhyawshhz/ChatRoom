#ifndef CHATROOM_NETWORK_EPOLLSERVER_H
#define CHATROOM_NETWORK_EPOLLSERVER_H

#include <boost/tuple/tuple.hpp>

class No1EpollServer
{
public:
	typedef boost::tuple<int> Request;
	enum
	{
		kPerReadSize				= 1024,
		kMaxHandleSize 				= 100000,
		kSockFdLen					= sizeof(int),
		kOneSockHeaderSize 			= kSockFdLen + sizeof(int),
		kOneSockCacheSize 			= 8096,
		kOneSockAllSize 			= kOneSockHeaderSize + kOneSockCacheSize 
	};

private:
	No1EpollServer();
	~No1EpollServer();

public:
	static No1EpollServer* get_instance();
	void start();
	void stop();

private:
	bool init();
	void release();


private:

	int													m_sock_fd;
	int													m_epoll_id;
	int													m_port;
	char*												m_buffer;
	static No1EpollServer*								m_instance;
};
#endif
