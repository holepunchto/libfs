#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__sparse (uv_file file) {
  HANDLE handle = uv_get_osfhandle(file);

  DWORD bytes;

  BOOL res = DeviceIoControl(
    handle,
    FSCTL_SET_SPARSE,
    NULL,
    0,
    NULL,
    0,
    &bytes, // Must be passed when lpOverlapped is NULL
    NULL
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}
