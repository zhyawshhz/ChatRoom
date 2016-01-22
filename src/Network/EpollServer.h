#ifndef CHATROOM_NETWORK_EPOLLSERVER_H
#define CHATROOM_NETWORK_EPOLLSERVER_H

#include <boost/tuple/tuple.hpp>
#include <boost/pool/object_pool.hpp>

#include "Util/Request.h"

class No1EpollServer : public No1ThrdObj
{
public:
	typedef boost::tuple<int> Request;
	enum
	{
		kHeaderSize					= 24,
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

	bool threadFunc(const int id);
	void add_msg(const msg &mm);
	bool remove_session(const int sockfd);

private:
	bool init();
	bool init_thrds();
	void release();
	bool proc_msg();
	bool proc_recv_client();


private:

	int													m_sock_fd;
	int													m_epoll_id;
	int													m_port;
	char*												m_buffer;
	bool 												m_stop;
	std::queue<msg>										m_read_msgs;
	std::map<int, msg>									m_send_msgs;
	boost::shared_ptr<No1Lock>							m_lock;
	boost::shared_ptr<No1CondVar>						m_cond_var;
	std::vector<No1Thread>								m_thrds;
	boost::object_pool<Request>							m_obj_pool;
	std::map<int, boost::shared_ptr<No1ServerSession> >	m_sessions;
	static No1EpollServer*								m_instance;
};
#endif
