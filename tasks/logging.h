#ifndef _TASKS_LOGGING_H_
#define _TASKS_LOGGING_H_

#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#ifdef LOGMUTEX
#include <mutex>
namespace tasks {
	extern std::mutex g_log_mutex;
}
#define _LOGMUTEX std::lock_guard<std::mutex> _log_lock(tasks::g_log_mutex)
#else
#define _LOGMUTEX
#endif

// Using the C time variants as std::put_time is not implemented yet.
#define tlog(s,m)														\
	{																	\
		_LOGMUTEX;														\
		s << "["														\
		  << std::setw(14)												\
		  << std::this_thread::get_id() << " "							\
		  << std::setw(16)												\
		  << __FILE__ << ":"											\
		  << std::setw(3)												\
		  << std::setfill('0')											\
		  << __LINE__ << "] "											\
		  << std::setfill(' ')											\
		  << m << std::flush;											\
	}
	
#ifdef DEBUG
#define tdbg(m) tlog(std::cout,m)
#else
#define tdbg(m)
#endif

#define terr(m) tlog(std::cerr,m)

#endif // _TASKS_LOGGING_H_
