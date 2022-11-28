#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__lock (uv_file file, int64_t offset, size_t length, bool shared) {
  struct flock data = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = shared ? F_RDLCK : F_WRLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(file, F_OFD_SETLKW, &data);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
