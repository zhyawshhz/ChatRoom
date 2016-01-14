/*
 * Lock.h
 *
 *  Created on: Jan 13, 2016
 *      Author: brian
 */

#ifndef CHATROOM_THREAD_LOCK_H_
#define CHATROOM_THREAD_LOCK_H_

#include <pthread.h>

class No1Lock
{
public:
	No1Lock();
	~No1Lock();

	bool lock();
	bool try_lock();
	bool unlock();


	inline pthread_mutex_t& get_id()const{return m_lock_id;}

private:
	pthread_mutex_t					m_lock_id;
};


#endif /* CHATROOM_THREAD_LOCK_H_ */
