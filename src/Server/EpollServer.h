#ifndef No1_Server_EpollServer_h
#define No1_Server_EpollServer_h
#include "Log/LogBaseClass.h"

class No1EpollServer : public No1LogBaseClass
{
public:
	enum
	{
		eMaxHandleSize 				= 100000
	};

private:
	No1EpollServer(const No1Config &config);
	~No1EpollServer();

public:
	static No1EpollServer* get_instance(const No1Config &config);

private:
	int									mSockFd;
	int									mEpollId;
	static No1EpollServer*				m_instance;
};
#endif
