/*
 * Request.h
 *
 *  Created on: Jan 18, 2016
 *      Author: brian
 */

#ifndef CHATROOM_UTIL_REQUEST_H_
#define CHATROOM_UTIL_REQUEST_H_

#include "proto/protocol.pb.h"

struct Request
{
public:
	int 		m_cfd;
	msg			m_msg;
};


#endif /* CHATROOM_UTIL_REQUEST_H_ */
