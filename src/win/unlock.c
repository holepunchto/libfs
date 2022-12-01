#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__unlock (uv_file file, int64_t offset, size_t length) {
  HANDLE handle = uv_get_osfhandle(file);

  if (length == 0) length = SIZE_MAX;

  OVERLAPPED data = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = UnlockFileEx(
    handle,
    0,
    length,
    length >> 32,
    &data
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}
