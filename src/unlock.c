#include <stddef.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

int
fs_unlock(uv_file file, int64_t offset, size_t length) {
  return fs__unlock(file, offset, length);
}
