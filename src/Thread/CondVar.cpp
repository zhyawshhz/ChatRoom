/*
 * CondVar.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: brian
 */
#include "Thread/CondVar.h"
#include "Thread/Lock.h"

No1CondVar::No1CondVar()
{
	pthread_cond_init(&m_condvar_id, NULL);
}


No1CondVar::~No1CondVar()
{
	pthread_cond_destroy(&m_cond_id);
}


bool
No1CondVar::wait(const No1Lock &lock)
{
	pthread_cond_wait(&m_condvar_id, &lock.get_id());
}


bool
No1CondVar::signal()
{
	pthread_cond_signal(&m_condvar_id);
}


bool
No1CondVar::signal_all()
{
	pthread_cond_broadcast(&m_condvar_id);
}



