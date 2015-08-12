#ifndef No1_Util_EpollServer_h
#define No1_Util_EpollServer_h

class No1EpollServer
{
private:
	explicit No1EpollServer();
	~No1EpollServer();

public:
	No1EpollServer* get_instance();
	void start();
};

#endif
