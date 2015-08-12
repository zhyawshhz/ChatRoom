#include <iostream>
#include "Log/Log.h"
#include "Log/TestLog.h"

int main()
{
	No1InitLog::init(true, true, static_cast<int>(debug));
	NO1_GLOBAL_LOG(debug, "I' m Testing!!!!");
	No1TestLog log;
	return 0;
}
