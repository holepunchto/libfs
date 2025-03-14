#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__trim(uv_file file, int64_t offset, size_t length) {
  HANDLE handle = uv_get_osfhandle(file);

  FILE_ZERO_DATA_INFORMATION data = {
    .FileOffset = {
      .QuadPart = offset,
    },
    .BeyondFinalZero = {
      .QuadPart = offset + length,
    },
  };

  DWORD bytes;

  BOOL res = DeviceIoControl(
    handle,
    FSCTL_SET_ZERO_DATA,
    &data,
    sizeof(data),
    NULL,
    0,
    &bytes, // Must be passed when lpOverlapped is NULL
    NULL
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}
