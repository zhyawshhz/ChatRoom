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
		if (size < 0)
		{
			if (errno == EINTR)
			{// interupt
				return handle_msg();
			}else{
				return dis_connect();
			}
		}else if (size == 0){
			return handle_msg();
		}else{
			m_body_len += size;
		}
	}
	return false;
}


bool
No1ServerSession::handle_msg()
{
	while (true)
	{
		int bodylen = *((int*)m_buffer);
		if (bodylen < 0)
		{
			GLOBAL_LOG_SEV(error, "BodyLen is error: " << m_addr << ", " << m_port);
			return dis_connect();
		}else if (bodylen == 0){
			handle_heartbeat();
		}else{

		}
	}
	return true;
}


bool
No1ServerSession::dis_connect()
{
	return true;
}


