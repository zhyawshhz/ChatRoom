/*
 * ServerSession.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: brian
 */

#include "Network/ServerSession.h"

#include "Log/Log.h"
#include "Util/NetworkUtil.h"
#include "proto/protocol.pb.h"

#include <string.h>


No1ServerSession::No1ServerSession(const int sockfd, const std::string &addr, const int port)
: m_sock_fd(sockfd)
, m_addr(addr)
, m_port(port)
, m_recv_len(0)
, m_send_len(0)
, m_recv_buffer(NULL)
, m_send_buffer(NULL)
, m_server(NULL)
{
	m_recv_buffer = new char[kBufferLen];
	m_send_buffer = new char[kBufferLen];
}


No1ServerSession::~No1ServerSession()
{
	if (m_recv_buffer) delete[] m_recv_buffer;
	if (m_send_buffer) delete[] m_send_buffer;
}


bool
No1ServerSession::recv_msg()
{
	while (true)
	{
		if (static_cast<int>(kBufferLen) - m_recv_len < static_cast<int>(kPerReadSize))
		{
			char* buffer = new char [2 * m_recv_len];
			memcpy(buffer, m_recv_buffer, m_recv_len);
			delete[] m_recv_buffer;
			m_recv_buffer = buffer;
		}

		int size = ::read(m_sock_fd, m_recv_buffer + m_recv_len, static_cast<int>(kPerReadSize));
		if (size  == -1)
		{
			if (errno == EAGAIN)
			{
				handle_msg();
				return true;
			} else {
				disconnect();
				return false;
			}
		}else if (size == 0){
			disconnect();
		} else {
			m_recv_len += size;
			continue;
		}
	}
	return false;
}


bool
No1ServerSession::handle_msg()
{
	int idx = 0;
	while (idx < m_recv_len)
	{
		int bodylen = *((int*)(m_recv_buffer+idx));
		if (bodylen < 0)
		{
			GLOBAL_LOG_SEV(error, "BodyLen is error: " << m_addr << ", " << m_port);
			memset(m_recv_buffer, 0, m_recv_len - idx);
			return false;
		}else if (bodylen == 0){
			handle_heartbeat();
		}else{
			msg mm = msg();
			mm.ParseFromArray(m_recv_buffer+idx+8, bodylen);
			idx +=(8+bodylen);
			m_server->add_msg(mm);
		}
	}
	return true;
}


bool
No1ServerSession::disconnect()
{
	m_server->remove_session(m_sock_fd);
	GLOBAL_LOG_SEV(info, "disconnect: " << m_addr << ", " << m_port);
	return true;
}


void
No1ServerSession::set_server(const No1EpollServer* server)
{
	m_server = server;
}


bool
No1ServerSession::add_msg(const char* buffer, const int len)
{
	if (m_send_len + len > static_cast<int>(kBufferLen))
	{
		char* temp_buffer = new char[2 * (m_send_len + len)];
		memcpy(temp_buffer, m_send_buffer, m_send_len);
		delete[] m_send_buffer;
		m_send_buffer = temp_buffer;
	}
	memcpy(m_send_buffer, buffer, len);
	m_send_len += len;
	return true;
}


bool
No1ServerSession::send_msg()
{
	int idx = 0;
	while (idx < m_send_len)
	{
		int size = ::write(m_sock_fd, m_send_buffer + idx, kPerReadSize);
		if (size == -1)
		{
			if (errno == EAGAIN)
			{
				sleep(1);
				continue;
			} else {
				disconnect();
			}
		} else if (size == 0) {
			disconnect();
		} else {
			idx += size;
		}
	}
	return true;
}

