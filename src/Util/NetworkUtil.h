/*
 * NetworkUtil.h
 *
 *  Created on: Jan 15, 2016
 *      Author: brian
 */

#ifndef CHATROOM_UTIL_NETWORKUTIL_H
#define CHATROOM_UTIL_NETWORKUTIL_H

#include <fcntl.h>
#include <netinet/in.h>


bool set_max_open_file(const int file_num)
{
	struct rlimit rt;
	rt.rlim_max = rt.rlim_cur = file_num;
	if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
	{
		return false;
	}
	return true;
}


bool set_reuse_addr(const int sockfd)
{
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		return false;
	}
	return true;
}


bool set_noblock(const int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) < 0)
	{
		return false;
	}
	return true;
}



#endif /* CHATROOM_UTIL_NETWORKUTIL_H */
