#define _GNU_SOURCE

#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

#ifndef RENAME_EXCHANGE
#define RENAME_EXCHANGE 2
#endif

int
fs__swap(const char *from, const char *to) {
  int res = syscall(SYS_renameat2, AT_FDCWD, from, AT_FDCWD, to, RENAME_EXCHANGE);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
