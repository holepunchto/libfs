#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"
#include "nt.h"

int
fs__get_attr(uv_file file, const char *name, uv_buf_t *value) {
  HANDLE handle = uv_get_osfhandle(file);
  HANDLE stream_handle;

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

  IO_STATUS_BLOCK status;

  NTSTATUS res = NtOpenFile(
    &stream_handle,
    GENERIC_READ,
    &object_attributes,
    &status,
    FILE_SHARE_READ,
    0
  );

  if (res < 0) {
    if (res == STATUS_OBJECT_NAME_NOT_FOUND) return UV_ENOENT;

    return UV_EIO;
  }

  FILE_STANDARD_INFORMATION info;

  res = NtQueryInformationFile(
    stream_handle,
    &status,
    &info,
    sizeof(info),
    FileStandardInformation
  );

  if (res < 0) {
    NtClose(stream_handle);

    return UV_EIO;
  }

  size_t length = info.EndOfFile.QuadPart;

  *value = uv_buf_init(malloc(length), length);

  LARGE_INTEGER offset = {
    .QuadPart = 0,
  };

  res = NtReadFile(
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

  if (res < 0) return UV_EIO;

  return 0;
}

int
fs__set_attr(uv_file file, const char *name, const uv_buf_t *value) {
  HANDLE handle = uv_get_osfhandle(file);
  HANDLE stream_handle;

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

  IO_STATUS_BLOCK status;

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

  if (res < 0) return UV_EIO;

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

  if (res < 0) return UV_EIO;

  return 0;
}

int
fs__remove_attr(uv_file file, const char *name) {
  HANDLE handle = uv_get_osfhandle(file);
  HANDLE stream_handle;

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

  IO_STATUS_BLOCK status;

  NTSTATUS res = NtOpenFile(
    &stream_handle,
    DELETE,
    &object_attributes,
    &status,
    FILE_SHARE_DELETE,
    0
  );

  if (res < 0) return UV_EIO;

  res = NtDeleteFile(&object_attributes);

  NtClose(stream_handle);

  if (res < 0) return UV_EIO;

  return 0;
}

int
fs__list_attrs(uv_file file, char **names, size_t *length) {
  HANDLE handle = uv_get_osfhandle(file);

  size_t info_size = 64;

  PFILE_STREAM_INFORMATION info = malloc(info_size);
  memset(info, 0, info_size);

  NTSTATUS res;

  do {
    IO_STATUS_BLOCK status;

    res = NtQueryInformationFile(
      handle,
      &status,
      &info[0],
      info_size,
      FileStreamInformation
    );

    if (res == STATUS_BUFFER_OVERFLOW) {
      info = realloc(info, info_size *= 2);
      memset(info, 0, info_size);
    } else {
      break;
    }
  } while (true);

  if (res < 0) return UV_EIO;

  PFILE_STREAM_INFORMATION next = info;

  size_t names_len = 0;
  *length = 0;

  do {
    PWCHAR unicode_name = &next->StreamName[1];

    unicode_name[wcslen(unicode_name) - wcslen(L":$DATA")] = L'\0';

    if (wcscmp(unicode_name, L"") != 0) {
      size_t name_len = WideCharToMultiByte(CP_UTF8, 0, unicode_name, -1, NULL, 0, NULL, NULL);

      *length += 1;

      names_len += sizeof(char *) + name_len;
    }

    if (next->NextEntryOffset) {
      next = (PFILE_STREAM_INFORMATION) (((uintptr_t) next) + next->NextEntryOffset);
    } else {
      break;
    }
  } while (true);

  *names = malloc(names_len);

  size_t offset = *length * sizeof(char *), i = 0, j = 0;

  next = info;

  do {
    PWCHAR unicode_name = &next->StreamName[1];

    if (wcscmp(unicode_name, L"") != 0) {
      size_t name_len = WideCharToMultiByte(CP_UTF8, 0, unicode_name, -1, NULL, 0, NULL, NULL);

      char *name = *names + offset + i;
      i += name_len;

      WideCharToMultiByte(CP_UTF8, 0, unicode_name, -1, name, name_len, NULL, NULL);

      memcpy(*names + j, &name, sizeof(char *));
      j += sizeof(char *);
    }

    if (next->NextEntryOffset) {
      next = (PFILE_STREAM_INFORMATION) (((uintptr_t) next) + next->NextEntryOffset);
    } else {
      break;
    }
  } while (true);

  free(info);

  return 0;
}
