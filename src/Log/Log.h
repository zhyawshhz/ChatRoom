#ifndef No1_Log_Log_h
#define No1_Log_Log_h

#include "Log/Init.h"

#if !defined NO1_GLOBAL_LOG
#	define NO1_GLOBAL_LOG(l, m) 				\
		BOOST_LOG_SEV(logger::get(), l) << m
#endif

#if !defined NO1_CLASS_LOG
#	define NO1_CLASS_LOG(l, m)					\
		BOOST_LOG_SEV(this->get_logger(), l) << m
#endif


#endif
