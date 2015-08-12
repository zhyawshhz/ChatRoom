#ifndef No1_Log_LogBaseClass_h
#define No1_Log_LogBaseClass_h

#include "Log/Init.h"

class No1LogBaseClass
{
public:
	No1LogBaseClass(){}
	virtual ~No1LogBaseClass(){}

public:
	No1LogType& get_logger(){return m_log;}

protected:
	No1LogType m_log;	
};
#endif

