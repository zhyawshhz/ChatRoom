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


No1ServerSession::No1ServerSession(const int sockfd, const std::string &addr, const int port)
: m_sock_fd(sockfd)
, m_addr(addr)
, m_port(port)
, m_body_len(0)
, m_buffer(NULL)
{
	m_buffer = new char[8*1024];
}


No1ServerSession::~No1ServerSession()
{
	if (m_buffer) delete[] m_buffer;
}


bool
No1ServerSession::proc()
{
	while (true)
	{
		int size = ::read(m_sock_fd, m_buffer + m_body_len, 1024);
		if (size  == -1)
		{
			if (errno == EAGAIN)
			{
				handle_msg();
				return true;
			} else {
				dis_connect();
				return false;
			}
		}else if (size == 0){
			dis_connect();
		} else {
			m_body_len += size;
			continue;
		}
	}
	return false;
}


bool
No1ServerSession::handle_msg()
{
	int idx = 0;
	while (idx < m_body_len)
	{
		int bodylen = *((int*)(m_buffer+idx));
		if (bodylen < 0)
		{
			GLOBAL_LOG_SEV(error, "BodyLen is error: " << m_addr << ", " << m_port);
			memset(m_buffer, 0, m_body_len - idx);
			return false;
		}else if (bodylen == 0){
			handle_heartbeat();
		}else{
			msg mm = msg();
			mm.ParseFromArray(m_buffer+idx+8, bodylen);
			idx +=(8+bodylen);
			m_server->add_msg(mm);
		}
	}
	return true;
}


bool
No1ServerSession::dis_connect()
{
	return true;
}


void
No1ServerSession::set_server(const No1EpollServer* server)
{
	m_server = server;
}


bool
No1ServerSession::send_msg(const char* buffer, const int len)
{
	return true;
}
