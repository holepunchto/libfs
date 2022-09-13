#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__downgrade_lock (uv_file file, int64_t offset, size_t length) {
  return fs__lock(file, offset, length, true);
}

int
fs__try_downgrade_lock (uv_file file, int64_t offset, size_t length) {
  return fs__try_lock(file, offset, length, true);
}

int
fs__upgrade_lock (uv_file file, int64_t offset, size_t length) {
  return fs__lock(file, offset, length, false);
}

int
fs__try_upgrade_lock (uv_file file, int64_t offset, size_t length) {
  return fs__try_lock(file, offset, length, false);
}
