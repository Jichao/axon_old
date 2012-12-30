
#include "stdheader.h"
#include "os_misc.h"
#include <sys/fcntl.h>
#include <sys/sysctl.h>

namespace axon {

//set nonblocking IO
int ax_set_nonblock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

//do shell command and read the output
//@return: -1  param error or file error
//         >0  number of read bytes
int ax_get_shell_result(const char* cmd, char* output, int maxn)
{
	int ret;
	FILE *fp;
	if (cmd == NULL) return -1;
	fp = popen(cmd, "r");
	if (fp == NULL) return -1;
	ret = fread(output, 1, maxn, fp);
	if (ret == maxn) {
		output[ret-1] = '\0';
	}
	pclose(fp);
	return ret;
}

int ax_get_physical_mem()
{
#if defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
	return (size_t)sysconf(_SC_PHYS_PAGES) * (size_t)sysconf(_SC_PAGESIZE);
#else
	return 0;
#endif
}


} //namespace
