//
//debug util and raw logger
//

#include "ax_debug.h"

namespace axon {

//RawLogger implementation ============================================

//path_root shared by all raw logger
char RawLogger::path_root[MAX_PATH_LEN] = ".";

RawLogger::RawLogger()
{
	memset(path_, 0, MAX_PATH_LEN);
	memset(file_, 0, MAX_FILENAME_LEN);
	strcpy(path_, ".");
	strcpy(file_, "debug.log");
}

RawLogger::RawLogger(const char* file)
{
	memset(path_, 0, MAX_PATH_LEN);
    memset(file_, 0, MAX_FILENAME_LEN);
	strcpy(path_, ".");
	strcpy(file_, "debug.log");
}

void RawLogger::set_pathroot(const char* p)
{
	strncpy(path_root, p, MAX_PATH_LEN-1);
}

void RawLogger::set_path(const char* p)
{
	strncpy(path_, p, MAX_PATH_LEN-1);
}

void RawLogger::set_logfile(const char* p)
{
	strncpy(file_, p, MAX_FILENAME_LEN-1);
}

//time head for logging
void RawLogger::fmt_time(char* buf, size_t n)
{
	time_t timep ;
	struct tm *tp;
	time(&timep);
	tp = localtime(&timep);
	strftime(buf, n, "%Y/%m/%d-%H:%M:%S", tp);
}

void RawLogger::vlog(const char* fmt, va_list arg)
{
	FILE *fp = NULL;
	static char path_buf[MAX_PATH_LEN * 2];
	char timebuf[64];	
	snprintf(path_buf, sizeof(path_buf), "%s/%s/%s", path_root, path_, file_);
	fp = fopen(path_buf, "a+");
	if (NULL == fp) {
		//perror(path_buf);
		//abort();
		return;
	}

	fmt_time(timebuf, 64);
	fprintf(fp, "%s ", timebuf);
	vfprintf(fp, fmt, arg);
	fprintf(fp, "\n");
	fflush(fp);
	fclose(fp);
}

void RawLogger::log(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlog(fmt, args);
	va_end(args);
}

//debug function ====================================================
//

static RawLogger g_dbg_logger("debug.log");
static RawLogger g_watch_logger;

void debug_log(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	g_dbg_logger.vlog(fmt, args);
	va_end(args);
}

void debug_watch(const char* codefile, const char* msg, ...)
{
	char fname[MAX_FILENAME_LEN];
	va_list args;
	va_start(args, msg);
	memset(fname, 0, sizeof(fname));
	snprintf(fname, sizeof(fname), "%s.log", codefile);
	g_watch_logger.set_logfile(fname);
	g_watch_logger.vlog(msg, args);
	va_end(args);
}


}
