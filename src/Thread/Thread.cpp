#include "Thread/Thread.h"

No1Thread::No1Thread(const int id, void*(start_func)(void*), boost::shared_ptr<No1ThrdObj> obj)
:
 mProcObj(obj)
,mId(id)
,mProcFunc(start_func)
{
	if (pthread_create(&mThrdId, NULL, threadFunc, NULL) != 0)
	{
		NO1_CLASS_LOG(error, "Failed to create thread: " << strerror(errno));
		return;
	}
}

No1Thread::~No1Thread()
{
}

void*
No1Thread::threadFunc(void* args)
{
	if (mProcObj) mProcObj->threadFunc();
}

void
No1Thread::join()
{
	void* res;
	if (pthread_join(mThrdId, &res) != 0)
	{
		NO1_CLASS_LOG(error, "Failed to join thread: " << strerror(errno));
		return;
	}
}

void
No1Thread::cancel()
{
	if (pthread_cancel(mThrdId) != 0)
	{
		NO1_CLASS_LOG(error, "Failed to cancel thread: " << mThrd);
		return;
	}
}
