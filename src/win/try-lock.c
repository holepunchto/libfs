#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__try_lock (uv_file file, int64_t offset, size_t length, bool shared) {
  if (length == 0) length = SIZE_MAX;

  DWORD flags = LOCKFILE_FAIL_IMMEDIATELY;

  if (!shared) flags |= LOCKFILE_EXCLUSIVE_LOCK;

  OVERLAPPED data = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = LockFileEx(
    file,
    flags,
    0,
    length,
    length >> 32,
    &data
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fs__try_downgrade_lock (uv_file file, int64_t offset, size_t length) {
  int res = fs__try_lock(file, offset, length, true);
  if (res < 0) return res;

  return fs__unlock(file, offset, length);
}

int
fs__try_upgrade_lock (uv_file file, int64_t offset, size_t length) {
  int res = fs__unlock(file, offset, length);
  if (res < 0) return res;

  return fs__try_lock(file, offset, length, false);
}
