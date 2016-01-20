/*
 * ServerSession.h
 *
 *  Created on: Jan 15, 2016
 *      Author: brian
 */

#ifndef CHATROOM_NETWORK_SERVERSESSION_H_
#define CHATROOM_NETWORK_SERVERSESSION_H_

class No1ServerSession
{
public:
	No1ServerSession(const int sockfd, const std::string &addr, const int port);
	~No1ServerSession();

public:
	bool 				proc();
	bool				dis_connect();
	bool				send_msg(const char* buffer, const int len);
	void 				set_server(const No1EpollServer* server);

private:
	bool				handle_msg();
	bool 				handle_heartbeat();

private:
	int					m_sock_fd;
	std::string			m_addr;
	int					m_port;
	int					m_body_len;
	char* 				m_buffer;
	No1EpollServer*		m_server;
};



#endif /* CHATROOM_NETWORK_SERVERSESSION_H_ */
