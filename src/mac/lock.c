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
  if (offset != 0 || length != 0) return UV_EINVAL;

  int res = flock(file, shared ? LOCK_SH : LOCK_EX);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
