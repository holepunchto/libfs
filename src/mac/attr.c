#include <errno.h>
#include <stdlib.h>
#include <sys/xattr.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__get_attr (uv_file file, const char *name, uv_buf_t *value) {
  int res = fgetxattr(file, name, NULL, 0, 0, 0);

  if (res == -1) return uv_translate_sys_error(errno);

  *value = uv_buf_init(malloc(res), res);

  res = fgetxattr(file, name, value->base, value->len, 0, 0);

  return res == -1 ? uv_translate_sys_error(errno) : 0;
}

int
fs__set_attr (uv_file file, const char *name, const uv_buf_t *value) {
  int res = fsetxattr(file, name, value->base, value->len, 0, 0);

  return res == -1 ? uv_translate_sys_error(errno) : 0;
}

int
fs__remove_attr (uv_file file, const char *name) {
  int res = fremovexattr(file, name, 0);

  return res == -1 ? uv_translate_sys_error(errno) : 0;
}

int
fs__list_attrs (uv_file file, char **names, size_t *length) {
  int res = flistxattr(file, NULL, 0, 0);

  if (res == -1) return uv_translate_sys_error(errno);

  *names = malloc(res);
  *length = res;

  res = flistxattr(file, *names, *length, 0);

  if (res == -1) return uv_translate_sys_error(errno);

  size_t i = 0, j = 0, n = *length;

  *length = 0;

  while (i < n) {
    i += strlen(*names + i) + 1;
    *length += 1;
  }

  size_t offset = *length * sizeof(char *);

  *names = realloc(*names, offset + n);

  memmove(*names + offset, *names, n);

  i = 0;

  while (i < n) {
    char *name = *names + offset + i;
    i += strlen(name) + 1;

    memcpy(*names + j, &name, sizeof(char *));
    j += sizeof(char *);
  }

  return 0;
}
