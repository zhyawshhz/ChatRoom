/*
 * Sem.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: brian
 */

#include "Thread/Sem.h"

No1Sem::No1Sem()
{

}

No1Sem::~No1Sem()
{
	sem_destroy(&m_sem_id);
}



