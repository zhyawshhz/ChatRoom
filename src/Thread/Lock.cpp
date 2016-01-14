/*
 * Lock.cpp
 *
 *  Created on: Jan 13, 2016
 *      Author: brian
 */

#include "Thread/Lock.h"
#include "Log/Log.h"

#include <errno.h>


No1Lock::No1Lock()
{
	pthread_mutex_init(&m_lock_id, NULL);
}


No1Lock::~No1Lock()
{
	pthread_mutex_destroy(&m_lock_id);
}


bool
No1Lock::lock()
{
	int ret = pthread_mutex_lock(&m_lock_id);
	if (ret != 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to lock: " << strerror(ret));
		return false;
	}
	return true;
}


bool
No1Lock::try_lock()
{
	int ret = pthread_mutex_trylock(&m_lock_id);
	if (ret == EBUSY) return false;
	if (ret != 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to trylock: " << strerror(ret));
		return false;
	}
	return true;
}


bool
No1Lock::unlock()
{
	int ret = pthread_mutex_unlock(&m_lock_id);
	if (ret != 0)
	{
		GLOBAL_LOG_SEV(error, "Failed to unlock: " << strerror(ret));
		return false;
	}
	return true;
}
