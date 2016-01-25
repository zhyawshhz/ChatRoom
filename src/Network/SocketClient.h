/*
 * SocketClient.h
 *
 *  Created on: Jan 25, 2016
 *      Author: brian
 */

#ifndef CHATROOM_NETWORK_SOCKETCLIENT_H_
#define CHATROOM_NETWORK_SOCKETCLIENT_H_
#include <sys/socket.h>


class No1SocketClient
{

public:
	enum
	{
		kPerReadSize					= 1024,
		kPerWriteSize					= 1024,
		kMaxBufferSize 					= 8 * 1024
	};


public:
	No1SocketClient(const std::string &addr, const int port);
	~No1SocketClient();

public:
	bool			recv(char*buffer, int &len);
	bool 			send(const char* buffer, const int len);
	bool			close();

private:
	bool			create_socket();
	bool			disconnect();

private:
	int				m_sock_fd;
	int				m_remote_port;
	std::string		m_remote_addr;
};




#endif /* CHATROOM_NETWORK_SOCKETCLIENT_H_ */
