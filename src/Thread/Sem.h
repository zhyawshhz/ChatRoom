/*
 * Sem.h
 *
 *  Created on: Jan 14, 2016
 *      Author: brian
 */

#ifndef CHATROOM_THREAD_SEM_H_
#define CHATROOM_THREAD_SEM_H_

#include <semaphore.h>

class No1Sem
{
public:
	No1Sem();
	~No1Sem();

private:
	sem_t				m_sem_id;
};


#endif /* CHATROOM_THREAD_SEM_H_ */
