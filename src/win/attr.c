#include <io.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"
#include "nt.h"

int
fs__get_attr (uv_file file, const char *name, uv_buf_t *value) {
  HANDLE handle = uv_get_osfhandle(file);
  HANDLE stream_handle;

  IO_STATUS_BLOCK status;

  size_t len = strlen(name);

  WCHAR unicode_name[MAX_PATH] = L":";
  MultiByteToWideChar(CP_OEMCP, 0, name, -1, &unicode_name[1], len + 1);

  len = wcslen(unicode_name) * 2;

  UNICODE_STRING object_name = {
    .Length = len,
    .MaximumLength = len + 2,
    .Buffer = unicode_name,
  };

  OBJECT_ATTRIBUTES object_attributes = {
    .Length = sizeof(object_attributes),
    .RootDirectory = handle,
    .ObjectName = &object_name,
  };

  NTSTATUS res = NtOpenFile(
    &stream_handle,
    GENERIC_READ,
    &object_attributes,
    &status,
    FILE_SHARE_READ,
    0
  );

  if (res < 0) return -1;

  NtClose(stream_handle);

  return 0;
}

int
fs__set_attr (uv_file file, const char *name, const uv_buf_t *value) {
  HANDLE handle = uv_get_osfhandle(file);
  HANDLE stream_handle;

  IO_STATUS_BLOCK status;

  size_t len = strlen(name);

  WCHAR unicode_name[MAX_PATH] = L":";
  MultiByteToWideChar(CP_OEMCP, 0, name, -1, &unicode_name[1], len + 1);

  len = wcslen(unicode_name) * 2;

  UNICODE_STRING object_name = {
    .Length = len,
    .MaximumLength = len + 2,
    .Buffer = unicode_name,
  };

  OBJECT_ATTRIBUTES object_attributes = {
    .Length = sizeof(object_attributes),
    .RootDirectory = handle,
    .ObjectName = &object_name,
  };

  NTSTATUS res = NtCreateFile(
    &stream_handle,
    GENERIC_WRITE,
    &object_attributes,
    &status,
    NULL,
    FILE_ATTRIBUTE_NORMAL,
    0,
    FILE_OVERWRITE_IF,
    0,
    NULL,
    0
  );

  if (res < 0) return -1;

  LARGE_INTEGER offset = {
    .QuadPart = 0,
  };

  res = NtWriteFile(
    stream_handle,
    NULL,
    NULL,
    NULL,
    &status,
    value->base,
    value->len,
    &offset,
    NULL
  );

  NtClose(stream_handle);

  if (res < 0) return -1;

  return 0;
}

int
fs__remove_attr (uv_file file, const char *name) {
  HANDLE handle = uv_get_osfhandle(file);

  return UV_ENOSYS;
}

int
fs__list_attrs (uv_file file, char **names, size_t *length) {
  HANDLE handle = uv_get_osfhandle(file);

  return UV_ENOSYS;
}
