#include <tasks/logging.h>

#ifdef LOGMUTEX
namespace tasks {
	std::mutex g_log_mutex;
}
#endif
