// Modified By Brian Zhang
// 01/06/2016
#include <iostream>
#include "Log/Log.h"
#include "Log/TestLog.h"
#include "Server/Config.h"
#include "Server/EpollServer.h"

int main(int argc, char** argv)
{
	No1Config config;
	config.parse(argc, argv);
	No1InitLog::init(config.log_toconsole, config.log_tofile, static_cast<int>(config.log_level));
	No1EpollServer::get_instance(config)->start();
	
	while (true)
	{
	
	}

	return 0;
}
