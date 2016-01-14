/*
 * CondVar.h
 *
 *  Created on: Jan 14, 2016
 *      Author: brian
 */

#ifndef CHATROOM_THREAD_CONDVAR_H_
#define CHATROOM_THREAD_CONDVAR_H_

#include <pthread.h>

class No1Lock;
class No1CondVar
{
public:
	No1CondVar();
	~No1CondVar();

public:
	bool wait(const No1Lock &lock);
	bool signal();
	bool signal_all();

private:
	pthread_cond_t					m_condvar_id;
};



#endif /* CHATROOM_THREAD_CONDVAR_H_ */
