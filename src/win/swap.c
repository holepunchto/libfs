#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

static int
fs__temp_path(char *path) {
  TCHAR temp_path[MAX_PATH];

  DWORD bytes = GetTempPath(MAX_PATH, temp_path);

  if (bytes == 0) return uv_translate_sys_error(GetLastError());

  bytes = GetTempFileName(temp_path, NULL, 0, path);

  return bytes > 0 ? 0 : uv_translate_sys_error(GetLastError());
}

static int
fs__move(const char *from, const char *to) {
  BOOL res = MoveFileEx(
    from,
    to,
    MOVEFILE_REPLACE_EXISTING
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fs__swap(const char *from, const char *to) {
  TCHAR swap[MAX_PATH];

  int err = fs__temp_path(swap);
  if (err < 0) return err;

  err = fs__move(to, swap);
  if (err < 0) return err;

  err = fs__move(from, to);
  if (err < 0) return err;

  return fs__move(swap, from);
}
