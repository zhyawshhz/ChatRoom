#include "Thread/Thread.h"
#include "Log/Log.h"

#include <pthread.h>

No1Thread::No1Thread(const int id, const boost::shared_ptr<No1ThrdObj> obj)
:
m_proc_obj(obj)
,m_id(id)
{
	if (pthread_create(&m_thrd_id, NULL, threadFunc, NULL) != 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to create thread: " << strerror(errno));
		return;
	}
}

No1Thread::~No1Thread()
{
}

void*
No1Thread::threadFunc(void* args)
{
	if (m_proc_obj)
	{
		m_proc_obj->threadFunc();
	}else{
		GLOBAL_LOG_SEV(error, "NO PROC OBJ: " << m_thrd_id);
		return NULL;
	}
}

void
No1Thread::join()
{
	void* res;
	if (pthread_join(m_thrd_id, &res) != 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to join thread: "<< strerror(errno));
		return;
	}
}

void
No1Thread::cancel()
{
	if (pthread_cancel(m_thrd_id) != 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to cancel thread: " << strerror(errno));
		return;
	}
}
