//
//Purpose: debug utilities and raw logger

#ifndef _AX_DEBUG_H_
#define _AX_DEBUG_H_

#include "common_def.h"
#include "stdheader.h"

namespace axon {

//runtime assert(enable both in debug and release enviornment)
#define RT_ASSERT(expr) (void)((expr) || (ax_assert(#expr, __FILE__, __FUNCTION__, __LINE__), 0))


//debug watch log. (output some info to help temp debugging, used for self test after writing new code)
#ifdef RUN_INTERNAL
#define DBG_WATCH(s) debug_watch(__FILE__, s)
#define DBG_WATCHV(s, ...) debug_watch(__FILE__, s, __VA_ARGS__)
//debug assert only in internal mode(ignore in release mode)
#define DBG_ASSERT(expr) (void)((expr) || (ax_assert(#expr, __FILE__, __FUNCTION__, __LINE__), 0))

#else
#define DBG_WATCH(s)
#define DBG_WATCHV(s, ...)
#define DBG_ASSERT(expr) 

#endif

enum DbgLevel 
{
	INFO = 0,
	WARNING = 1,
	ERROR = 2,
	FATAL = 3,	
};

//raw logger, write directedly to local file system
//used for code debug and runtime info
class RawLogger 
{
public:
	RawLogger();
	RawLogger(const char* file);
	static void set_pathroot(const char* p);
	void set_path(const char* path);
	void set_logfile(const char* file);
	void log(const char* fmt, ...);
	void vlog(const char* fmt, va_list arg);
	void fmt_time(char* buf, size_t n);

protected:
	static char path_root[MAX_PATH_LEN];   //shared by all raw logger
	char path_[MAX_PATH_LEN];
	char file_[MAX_FILENAME_LEN];
};

void ax_assert(const char *expr, const char* file, const char* func, int line);
void debug_log(DbgLevel lv, const char* fmt, ...);
void debug_watch(const char *codefile,  const char* msg, ...);


}


#endif

