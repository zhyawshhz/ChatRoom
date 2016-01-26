/*
 * main.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: brian
 */
#include "Config/Config.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	std::string addr = No1Config::get_instance()->get_server_addr();
	int port = No1Config::get_instance()->get_server_port();
	std::vector<No1ClientSession> sessions;
	int p_num = No1Config::get_instance()->get_process_num();
	for (int i=0; i<p_num; ++i)
	{
		pid_t p = fork();
		if (p == 0)
		{
			No1ClientSession session(addr, port);
			sessions.push_back(session);
			session.run();
			return 0;
		}
		waitpid(p, NULL, 0);
	}
	return 0;
}




