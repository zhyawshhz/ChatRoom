/*
 * ClientSession.h
 *
 *  Created on: Jan 25, 2016
 *      Author: brian
 */

#ifndef CHATROOM_NETWORK_CLIENTSESSION_H_
#define CHATROOM_NETWORK_CLIENTSESSION_H_

#include <boost/shared_ptr.hpp>
#include "Thread/ThrdObj.h"


class No1SocketClient;
class No1ClientSession : public No1ThrdObj
{
public:
	enum
	{
		kMaxBufferSize				= 8*1024
	};

public:
	No1ClientSession(const std::string &addr, const int port);
	virtual ~No1ClientSession();

public:
	bool						run();
	bool						close();
	bool						threadFunc(const int id);


private:
	bool						handle_msg(const char* buffer, const int len);
	bool						handle_recv();
	bool						handle_send();

private:
	bool										m_stop;
	std::string									m_user_name;
	boost::shared_ptr<No1Thread>				m_send_thrd;
	boost::shared_ptr<No1Thread>				m_recv_thrd;
	boost::shared_ptr<No1SocketClient>			m_sock;

};



#endif /* CHATROOM_NETWORK_CLIENTSESSION_H_ */
