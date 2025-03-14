#ifndef FS_H
#define FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uv.h>

typedef struct fs_open_s fs_open_t;
typedef struct fs_access_s fs_access_t;
typedef struct fs_close_s fs_close_t;
typedef struct fs_read_s fs_read_t;
typedef struct fs_read_batch_s fs_read_batch_t;
typedef struct fs_write_s fs_write_t;
typedef struct fs_write_batch_s fs_write_batch_t;
typedef struct fs_stat_s fs_stat_t;
typedef struct fs_truncate_s fs_truncate_t;
typedef struct fs_trim_s fs_trim_t;
typedef struct fs_sparse_s fs_sparse_t;
typedef struct fs_chmod_s fs_chmod_t;
typedef struct fs_lock_s fs_lock_t;
typedef struct fs_get_attr_s fs_get_attr_t;
typedef struct fs_set_attr_s fs_set_attr_t;
typedef struct fs_remove_attr_s fs_remove_attr_t;
typedef struct fs_list_attrs_s fs_list_attrs_t;
typedef struct fs_realpath_s fs_realpath_t;
typedef struct fs_mkdir_s fs_mkdir_t;
typedef struct fs_rmdir_s fs_rmdir_t;
typedef struct fs_symlink_s fs_symlink_t;
typedef struct fs_unlink_s fs_unlink_t;
typedef struct fs_rename_s fs_rename_t;
typedef struct fs_swap_s fs_swap_t;
typedef struct fs_merge_s fs_merge_t;

typedef void (*fs_open_cb)(fs_open_t *req, int status, uv_file file);
typedef void (*fs_access_cb)(fs_access_t *req, int status);
typedef void (*fs_close_cb)(fs_close_t *req, int status);
typedef void (*fs_read_cb)(fs_read_t *req, int status, size_t len);
typedef void (*fs_read_batch_cb)(fs_read_batch_t *req, int status, size_t len);
typedef void (*fs_write_cb)(fs_write_t *req, int status, size_t len);
typedef void (*fs_write_batch_cb)(fs_write_batch_t *req, int status, size_t len);
typedef void (*fs_stat_cb)(fs_stat_t *req, int status, const uv_stat_t *stat);
typedef void (*fs_truncate_cb)(fs_truncate_t *req, int status);
typedef void (*fs_trim_cb)(fs_trim_t *req, int status);
typedef void (*fs_sparse_cb)(fs_sparse_t *req, int status);
typedef void (*fs_chmod_cb)(fs_chmod_t *req, int status);
typedef void (*fs_lock_cb)(fs_lock_t *req, int status);
typedef void (*fs_get_attr_cb)(fs_get_attr_t *req, int status, const uv_buf_t *value);
typedef void (*fs_set_attr_cb)(fs_set_attr_t *req, int status);
typedef void (*fs_remove_attr_cb)(fs_remove_attr_t *req, int status);
typedef void (*fs_list_attrs_cb)(fs_list_attrs_t *req, int status, const char *attrs[], ssize_t len);
typedef void (*fs_realpath_cb)(fs_realpath_t *req, int status, const char *path);
typedef void (*fs_mkdir_cb)(fs_mkdir_t *req, int status);
typedef void (*fs_rmdir_cb)(fs_rmdir_t *req, int status);
typedef void (*fs_symlink_cb)(fs_symlink_t *req, int status);
typedef void (*fs_unlink_cb)(fs_unlink_t *req, int status);
typedef void (*fs_rename_cb)(fs_rename_t *req, int status);
typedef void (*fs_swap_cb)(fs_swap_t *req, int status);
typedef void (*fs_merge_cb)(fs_merge_t *req, int status);

struct fs_open_s {
  uv_fs_t req;

  int flags;
  int mode;

  fs_open_cb cb;

  void *data;
};

struct fs_access_s {
  uv_fs_t req;

  int mode;

  fs_access_cb cb;

  void *data;
};

struct fs_close_s {
  uv_fs_t req;
  uv_file file;

  fs_close_cb cb;

  void *data;
};

struct fs_read_s {
  uv_fs_t req;
  uv_file file;

  fs_read_cb cb;

  void *data;
};

struct fs_read_batch_s {
  uv_fs_t req;
  uv_file file;

  const uv_buf_t *bufs;
  const int64_t *offsets;

  size_t batched;
  size_t remaining;
  size_t len;

  fs_read_batch_cb cb;

  void *data;
};

struct fs_write_s {
  uv_fs_t req;
  uv_file file;

  fs_write_cb cb;

  void *data;
};

struct fs_write_batch_s {
  uv_fs_t req;
  uv_file file;

  const uv_buf_t *bufs;
  const int64_t *offsets;

  size_t batched;
  size_t remaining;
  size_t len;

  fs_write_batch_cb cb;

  void *data;
};

struct fs_stat_s {
  uv_fs_t req;
  uv_file file;

  fs_stat_cb cb;

  void *data;
};

struct fs_truncate_s {
  uv_fs_t req;
  uv_file file;

  fs_truncate_cb cb;

  void *data;
};

struct fs_trim_s {
  uv_work_t req;
  uv_file file;

  int64_t offset;
  uint64_t length;

  fs_trim_cb cb;

  int result;

  void *data;
};

struct fs_sparse_s {
  uv_work_t req;
  uv_file file;

  fs_sparse_cb cb;

  int result;

  void *data;
};

struct fs_chmod_s {
  uv_fs_t req;
  uv_file file;

  int mode;

  fs_chmod_cb cb;

  void *data;
};

struct fs_lock_s {
  uv_work_t req;
  uv_file file;

  int64_t offset;
  uint64_t length;
  bool shared;

  fs_lock_cb cb;

  int result;

  void *data;
};

struct fs_get_attr_s {
  uv_work_t req;
  uv_file file;

  const char *name;
  uv_buf_t value;

  fs_get_attr_cb cb;

  int result;

  void *data;
};

struct fs_set_attr_s {
  uv_work_t req;
  uv_file file;

  const char *name;
  const uv_buf_t *value;

  fs_set_attr_cb cb;

  int result;

  void *data;
};

struct fs_remove_attr_s {
  uv_work_t req;
  uv_file file;

  const char *name;

  fs_remove_attr_cb cb;

  int result;

  void *data;
};

struct fs_list_attrs_s {
  uv_work_t req;
  uv_file file;

  char *names;
  size_t length;

  fs_list_attrs_cb cb;

  int result;

  void *data;
};

struct fs_realpath_s {
  uv_fs_t req;

  fs_realpath_cb cb;

  void *data;
};

struct fs_mkdir_s {
  uv_fs_t req;
  char *path;

  int mode;

  fs_mkdir_cb cb;

  void *data;
};

struct fs_rmdir_s {
  uv_fs_t req;
  char *path;

  int mode;

  fs_rmdir_cb cb;

  int result;

  void *data;
};

struct fs_symlink_s {
  uv_fs_t req;

  int flags;

  fs_symlink_cb cb;

  void *data;
};

struct fs_unlink_s {
  uv_fs_t req;

  fs_unlink_cb cb;

  void *data;
};

struct fs_rename_s {
  uv_fs_t req;

  fs_rename_cb cb;

  void *data;
};

struct fs_swap_s {
  uv_work_t req;

  char *from;
  char *to;

  fs_swap_cb cb;

  int result;

  void *data;
};

struct fs_merge_s {
  uv_fs_t req;

  bool replace;

  fs_merge_cb cb;

  int result;

  void *data;
};

int
fs_open(uv_loop_t *loop, fs_open_t *req, const char *path, int flags, int mode, fs_open_cb cb);

int
fs_access(uv_loop_t *loop, fs_access_t *req, const char *path, int mode, fs_access_cb cb);

int
fs_close(uv_loop_t *loop, fs_close_t *req, uv_file file, fs_close_cb cb);

int
fs_read(uv_loop_t *loop, fs_read_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_read_cb cb);

int
fs_read_batch(uv_loop_t *loop, fs_read_batch_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[], fs_read_batch_cb cb);

int
fs_write(uv_loop_t *loop, fs_write_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_write_cb cb);

int
fs_write_batch(uv_loop_t *loop, fs_write_batch_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[], fs_write_batch_cb cb);

int
fs_stat(uv_loop_t *loop, fs_stat_t *req, uv_file file, fs_stat_cb cb);

int
fs_truncate(uv_loop_t *loop, fs_truncate_t *req, uv_file file, int64_t offset, fs_truncate_cb cb);

int
fs_trim(uv_loop_t *loop, fs_trim_t *req, uv_file file, int64_t offset, size_t length, fs_trim_cb cb);

int
fs_sparse(uv_loop_t *loop, fs_sparse_t *req, uv_file file, fs_sparse_cb cb);

int
fs_chmod(uv_loop_t *loop, fs_chmod_t *req, uv_file file, int mode, fs_chmod_cb cb);

int
fs_lock(uv_loop_t *loop, fs_lock_t *req, uv_file file, int64_t offset, size_t length, bool shared, fs_lock_cb cb);

int
fs_try_lock(uv_file file, int64_t offset, size_t length, bool shared);

int
fs_downgrade_lock(uv_loop_t *loop, fs_lock_t *req, uv_file file, int64_t offset, size_t length, fs_lock_cb cb);

int
fs_try_downgrade_lock(uv_file file, int64_t offset, size_t length);

int
fs_upgrade_lock(uv_loop_t *loop, fs_lock_t *req, uv_file file, int64_t offset, size_t length, fs_lock_cb cb);

int
fs_try_upgrade_lock(uv_file file, int64_t offset, size_t length);

int
fs_unlock(uv_file file, int64_t offset, size_t length);

int
fs_get_attr(uv_loop_t *loop, fs_get_attr_t *req, uv_file file, const char *name, fs_get_attr_cb cb);

int
fs_set_attr(uv_loop_t *loop, fs_set_attr_t *req, uv_file file, const char *name, const uv_buf_t *value, fs_set_attr_cb cb);

int
fs_remove_attr(uv_loop_t *loop, fs_remove_attr_t *req, uv_file file, const char *name, fs_remove_attr_cb cb);

int
fs_list_attrs(uv_loop_t *loop, fs_list_attrs_t *req, uv_file file, fs_list_attrs_cb cb);

int
fs_realpath(uv_loop_t *loop, fs_realpath_t *req, const char *path, fs_realpath_cb cb);

int
fs_mkdir(uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, bool recursive, fs_mkdir_cb cb);

int
fs_rmdir(uv_loop_t *loop, fs_rmdir_t *req, const char *path, bool recursive, fs_rmdir_cb cb);

int
fs_symlink(uv_loop_t *loop, fs_symlink_t *req, const char *target, const char *link, int flags, fs_symlink_cb);

int
fs_unlink(uv_loop_t *loop, fs_unlink_t *req, const char *path, fs_unlink_cb cb);

int
fs_rename(uv_loop_t *loop, fs_rename_t *req, const char *from, const char *to, fs_rename_cb cb);

int
fs_swap(uv_loop_t *loop, fs_swap_t *req, const char *from, const char *to, fs_swap_cb cb);

int
fs_merge(uv_loop_t *loop, fs_merge_t *req, const char *base, const char *onto, bool replace, fs_merge_cb cb);

#ifdef __cplusplus
}
#endif
#endif // FS_H
