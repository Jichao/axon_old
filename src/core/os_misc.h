
#ifndef _AX_OS_MISC_H_
#define _AX_OS_MISC_H_

namespace axon {

//set nonblocking IO
int ax_set_nonblock(int fd);

//do shell command and read the output
int ax_get_shell_result(const char* cmd, char* output, int maxn);

int ax_get_physical_mem();

} //namespace

#endif
