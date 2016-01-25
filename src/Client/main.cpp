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

int main(int argc, char** argv)
{
	int p_num = No1Config::get_instance()->get_process_num();
	for (int i=0; i<p_num; ++i)
	{
		pid_t p = fork();
		if (p == 0)
		{

		}
	}
	return 0;
}




