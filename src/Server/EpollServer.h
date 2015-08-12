#ifndef No1_Server_EpollServer_h
#define No1_Server_EpollServer_h
#include "Log/LogBaseClass.h"
#include <boost/tuple/tuple.hpp>

class No1EpollServer : public No1LogBaseClass
{
public:
	typedef boost::tuple<int> Request;
	enum
	{
		kPerReadSize				= 1024,
		kMaxHandleSize 				= 100000,
		kSockFdLen					= sizeof(int),
		kOneSockHeaderSize 			= kSockFdLen + sizeof(int);
		kOneSockCacheSize 			= 8096,
		kOneSockAllSize 			= kOneSockHeaderSize + kOneSockCacheSize 
	};

private:
	No1EpollServer(const No1Config &config);
	~No1EpollServer();

public:
	static No1EpollServer* get_instance(const No1Config &config);

private:

	int													mSockFd;
	int													mEpollId;
	std::uint32_t										mPort;
	std::uint32_t										mCrtReqSize;
	boost::shared_array<char>							mBuffer;
	static No1EpollServer*								mInstance;
};
#endif
