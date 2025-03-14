#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

int
fs_try_lock(uv_file file, int64_t offset, size_t length, bool shared) {
  int err = fs__try_lock(file, offset, length, shared);

  if (err == UV_EACCES || err == UV_EAGAIN || err == UV_EBUSY) {
    err = UV_EAGAIN;
  }

  return err;
}

int
fs_try_downgrade_lock(uv_file file, int64_t offset, size_t length) {
  int err = fs__try_downgrade_lock(file, offset, length);

  if (err == UV_EACCES || err == UV_EAGAIN || err == UV_EBUSY) {
    err = UV_EAGAIN;
  }

  return err;
}

int
fs_try_upgrade_lock(uv_file file, int64_t offset, size_t length) {
  int err = fs__try_upgrade_lock(file, offset, length);

  if (err == UV_EACCES || err == UV_EAGAIN || err == UV_EBUSY) {
    err = UV_EAGAIN;
  }

  return err;
}
