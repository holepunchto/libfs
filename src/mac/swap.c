#include <errno.h>
#include <fcntl.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__swap(const char *from, const char *to) {
  int res = renameatx_np(AT_FDCWD, from, AT_FDCWD, to, RENAME_SWAP);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
