#ifndef FS_PLATFORM_H
#define FS_PLATFORM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fs.h"

int
fs__trim (uv_file file, int64_t offset, size_t length);

int
fs__sparse (uv_file file);

int
fs__lock (uv_file file, int64_t offset, size_t length, bool shared);

int
fs__try_lock (uv_file file, int64_t offset, size_t length, bool shared);

int
fs__downgrade_lock (uv_file file, int64_t offset, size_t length);

int
fs__try_downgrade_lock (uv_file file, int64_t offset, size_t length);

int
fs__upgrade_lock (uv_file file, int64_t offset, size_t length);

int
fs__try_upgrade_lock (uv_file file, int64_t offset, size_t length);

int
fs__unlock (uv_file file, int64_t offset, size_t length);

int
fs__swap (const char *from, const char *to);

#endif // FS_PLATFORM_H
