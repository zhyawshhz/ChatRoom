/*
 * SocketClient.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: brian
 */

#include "Network/SocketClient.h"

#include "Log/Log.h"
#include "Util/NetworkUtil.h"


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

No1SocketClient::No1SocketClient(const std::string &addr, const int port)
: m_sock_fd(-1)
, m_remote_addr(addr)
, m_remote_port(port)
{
	create_socket();
}


No1SocketClient::~No1SocketClient()
{
	if (m_sock_fd > 0) ::close(m_sock_fd);
}


bool
No1SocketClient::create_socket()
{
	m_sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock_fd < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to create socket: " << strerror(errno));
		return false;
	}

	if (!set_noblock(m_sock_fd))
	{
		GLOBAL_LOG_SEV(error, "Failed to set noblock!!!!");
		return false;
	}

	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);
	client_addr.sin_port = htons(0);


	if (bind(m_sock_fd, (struct sockaddr*)(&client_addr), sizeof(struct sockaddr)) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to bind socket: " << strerror(errno));
		return false;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_aton(m_remote_addr.data(), &(server_addr.sin_addr));
	server_addr.sin_port = htons(m_remote_port);
	if (connect(m_sock_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to connect to server: " << strerror(errno));
		return false;
	}
	return true;
}


bool
No1SocketClient::send(const char* buffer, const int len)
{
	int idx = 0;
	while (idx < len)
	{
		int size = ::write(m_sock_fd, buffer+idx, kPerWriteSize);
		if (size < 0)
		{
			if (errno == EAGAIN)
			{
				sleep(1);
				continue;
			} else {
				disconnect();
				return false;
			}
		} else if (size == 0){
			disconnect();
			return false;
		} else {
			idx += size;
		}
	}
	return true;
}


bool
No1SocketClient::recv(char* buffer, int &len)
{
	int idx = 0;
	while (true)
	{
		if ((idx += kPerReadSize) > kMaxBufferSize)
		{
			GLOBAL_LOG_SEV(warning, "buffer is fill, read next time!!!!");
			len = idx;
			return true;
		}

		int size = ::read(m_sock_fd, buffer + idx, kPerReadSize);
		if (size < 0)
		{
			if (errno == EAGAIN)
			{
				len = idx;
				return true;
			} else {
				disconnect();
				return false;
			}

		}else if (size == 0){
			disconnect();
			return false;
		} else {
			idx += size;
		}
	}
	return true;
}


bool
No1SocketClient::disconnect()
{
	int ret = ::close(m_sock_fd);
	if (ret < 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to close socket: " << strerror(errno));
		return false;
	}
	return true;
}


bool
No1SocketClient::close()
{
	return disconnect();
}



