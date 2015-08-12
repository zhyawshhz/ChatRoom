#include "Util/EpollServer.h"

No1EpollServer::No1EpollServer()
{
	std::cout << "Epoll Server start ..." << std::endl;
}

No1EpollServer::~No1EpollServer()
{
	if (m_instance) delete m_instance;
	std::cout << "Epoll Server shutdown ..." << std::endl;
}

No1EpollServer*
No1EpollServer::get_instance()
{
	if (m_instance) return m_instance;
	m_instance = new No1EpollServer();
	if (!m_instance)
	{
		std::cerr << "Failed to start epoll server!!!" << std::endl;
		return NULL;
	}
	return m_instance;
}
