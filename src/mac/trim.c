#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <uv.h>

#include "../../include/fs.h"
#include "../platform.h"

static inline size_t
fs__align_up (size_t n, size_t align) {
  return (n + (align - 1)) & -align;
}

static inline size_t
fs__align_down (size_t n, size_t align) {
  return n & -align;
}

static inline size_t
fs__min (size_t a, size_t b) {
  return a < b ? a : b;
}

int
fs__trim (uv_file file, int64_t offset, size_t length) {
  struct stat st;

  int res = fstat(file, &st);

  if (res == -1) return uv_translate_sys_error(errno);

  size_t align = st.st_blksize;

  size_t aligned_offset = fs__align_up(offset, align);

  if (aligned_offset != offset) {
    size_t zero_length = fs__min(length, aligned_offset - offset);

    void *buf = calloc(1, zero_length);

    ssize_t res = pwrite(file, buf, zero_length, offset);

    free(buf);

    if (res == -1) return uv_translate_sys_error(errno);

    offset += zero_length;
    length -= zero_length;
  }

  size_t aligned_length = fs__align_down(length, align);

  if (aligned_length >= align) {
    struct fpunchhole data = {
      .fp_flags = 0,
      .reserved = 0,
      .fp_offset = offset,
      .fp_length = aligned_length,
    };

    int res = fcntl(file, F_PUNCHHOLE, &data);

    if (res == -1) return uv_translate_sys_error(errno);

    offset += aligned_length;
    length -= aligned_length;
  }

  if (length > 0) {
    void *buf = calloc(1, length);

    ssize_t res = pwrite(file, buf, length, offset);

    free(buf);

    if (res == -1) return uv_translate_sys_error(errno);
  }

  return 0;
}
