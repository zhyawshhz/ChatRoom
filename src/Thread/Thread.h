#ifndef No1_Thread_Thread_h
#define No1_Thread_Thread_h

#include <pthread.h>

#include "Thread/ThrdObj.h"

class No1Thread
{
public:
	No1Thread(const int id, const boost::shared_ptr<No1ThrdObj> obj);
	~No1Thread();

private:
	static void* threadFunc(void* args);
	void join();
	void cancel();

private:
	int										m_id;
	pthread_t								m_thrd_id;
	boost::shared_ptr<No1ThrdObj>			m_proc_obj;
};

#endif
