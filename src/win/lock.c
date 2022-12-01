#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__lock (uv_file file, int64_t offset, size_t length, bool shared) {
  HANDLE handle = uv_get_osfhandle(file);

  if (length == 0) length = SIZE_MAX;

  DWORD flags = 0;

  if (!shared) flags |= LOCKFILE_EXCLUSIVE_LOCK;

  OVERLAPPED data = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = LockFileEx(
    handle,
    flags,
    0,
    length,
    length >> 32,
    &data
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fs__downgrade_lock (uv_file file, int64_t offset, size_t length) {
  int res = fs__lock(file, offset, length, true);
  if (res < 0) return res;

  return fs__unlock(file, offset, length);
}

int
fs__upgrade_lock (uv_file file, int64_t offset, size_t length) {
  int res = fs__unlock(file, offset, length);
  if (res < 0) return res;

  return fs__lock(file, offset, length, false);
}
