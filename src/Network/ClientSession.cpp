/*
 * ClientSession.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: brian
 */

#include "Network/ClientSession.h"

#include "Network/SocketClient.h"
#include "Thread/Thread.h"
#include "Log/Log.h"
#include "proto/protocol.pb.h"
#include "Util/Random.h"


#include <iostream>

No1ClientSession::No1ClientSession(const std::string &addr, const int port)
: m_stop(true)
{
	char user_name[12+1];
	get_random_str(user_name, 12);
	m_user_name = std::string(user_name, 12);
	m_sock = boost::make_shared<No1SocketClient>(addr, port);
}


No1ClientSession::~No1ClientSession()
{
	m_sock->close();
}


bool
No1ClientSession::run()
{
	m_send_thrd = boost::make_shared<No1Thread>(0, this);
	m_recv_thrd = boost::make_shared<No1Thread>(1, this);
	m_send_thrd->join();
	m_recv_thrd->join();
	return true;
}


bool
No1ClientSession::close()
{
	m_stop = true;
	m_sock->close();
	return true;
}


bool
No1ClientSession::threadFunc(const int id)
{
	if (id == 0)
	{
		return handle_send();
	}

	if (id == 1)
	{
		return handle_recv();
	}
	return true;
}


bool
No1ClientSession::handle_recv()
{
	int len = 0;
	char buffer[kMaxBufferSize];
	while (!m_stop)
	{
		memset(buffer, 0, kMaxBufferSize);
		if (!m_sock->recv(buffer, len))
		{
			GLOBAL_LOG_SEV(error, "socket is close!!!!");
			close();
			continue;
		}
		handle_msg(buffer, len);
	}
	return true;
}


bool
No1ClientSession::handle_send()
{
	srand(static_cast<int>(time(0)));
	int str_len = rand() % 512;
	char send_msg[str_len+1];
	get_random_str(send_msg, str_len);

	No1Request request;
	request.set_msg(send_msg);
	request.set_username(m_user_name);

	int bodylen = request.ByteSize();
	char send_buffer[8+bodylen];
	(*((int*)send_buffer)) = bodylen;
	(*((int*)send_buffer+4)) = 1;
	request.SerializeToArray(send_buffer+8, bodylen);

	if (!m_sock->send(send_buffer, bodylen+8))
	{
		GLOBAL_LOG_SEV(error, "socket is close!!!!");
		close();
		return false;
	}

	return true;
}


bool
No1ClientSession::handle_msg(const char* buffer, const int len)
{
	int idx = 0;
	while (idx < len)
	{
		int p_len = (*((int*)(buffer+idx)));
		No1Request request;
		request.ParseFromArray(buffer+idx+8, p_len);
		std::cout << "Receive msg: " << request.msg() << std::endl;
		idx += (p_len + 8);
	}
	return true;
}
