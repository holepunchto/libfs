#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

int
fs__get_attr (uv_file file, const char *name, uv_buf_t *value) {
  return UV_ENOSYS;
}

int
fs__set_attr (uv_file file, const char *name, const uv_buf_t *value) {
  return UV_ENOSYS;
}

int
fs__remove_attr (uv_file file, const char *name) {
  return UV_ENOSYS;
}

int
fs__list_attrs (uv_file file, char **names, size_t *length) {
  return UV_ENOSYS;
}
