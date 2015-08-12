#include "Log/TestLog.h"
#include "Log/Log.h"

No1TestLog::No1TestLog()
{
	int i = 0;
	while (i++ < 100000000)
	{
		NO1_CLASS_LOG(debug, "TestLogClass");
	}
}

No1TestLog::~No1TestLog()
{

}
