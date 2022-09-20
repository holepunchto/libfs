#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) &S_IFMT) == S_IFDIR)
#endif

static inline size_t
last_path_sep (const char *path) {
  size_t len = strlen(path);

  while (len > 0) {
    char c = path[len - 1];

#ifdef __POSIX__
    if (c == '/') {
#else
    if (c == '/' || c == '\\') {
#endif
      return len - 1;
    }

    len--;
  }

  return (size_t) -1;
}

static void
on_stat (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  int err = req->result;

  if (err == 0 && !S_ISDIR(req->statbuf.st_mode)) {
    err = UV_ENOTDIR;
  }

  mkdir_req->cb(mkdir_req, err);

  if (mkdir_req->next != NULL) free(mkdir_req->next);

  uv_fs_req_cleanup(req);
}

static void
on_mkdir (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  int err = req->result;

  if (mkdir_req->recursive) {
    switch (err) {
    case UV_EACCES:
    case UV_ENOSPC:
    case UV_ENOTDIR:
    case UV_EPERM:
      break;

    case 0: {
      if (mkdir_req->next == NULL || strlen(mkdir_req->next) == strlen(mkdir_req->path)) {
        break;
      }

      size_t len = strlen(mkdir_req->next);

      mkdir_req->next[len] = '/';

      uv_fs_req_cleanup(req);

      err = uv_fs_mkdir(req->loop, req, mkdir_req->next, mkdir_req->mode, on_mkdir);

      if (err == 0) return;

      break;
    }

    case UV_ENOENT: {
      size_t n = last_path_sep(req->path);

      if (n == -1) break;

      if (mkdir_req->next == NULL) {
        mkdir_req->next = strdup(mkdir_req->path);
      }

      mkdir_req->next[n] = '\0';

      uv_fs_req_cleanup(req);

      err = uv_fs_mkdir(req->loop, req, mkdir_req->next, mkdir_req->mode, on_mkdir);

      if (err == 0) return;

      break;
    }

    default:
      uv_fs_req_cleanup(req);

      err = uv_fs_stat(req->loop, req, mkdir_req->path, on_stat);

      if (err == 0) return;
    }
  }

  mkdir_req->cb(mkdir_req, err);

  if (mkdir_req->next != NULL) free(mkdir_req->next);

  uv_fs_req_cleanup(req);
}

int
fs_mkdir (uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, bool recursive, fs_mkdir_cb cb) {
  req->req.data = req;
  req->path = path;
  req->next = NULL;
  req->mode = mode;
  req->recursive = recursive;
  req->cb = cb;

  return uv_fs_mkdir(loop, &req->req, path, mode, on_mkdir);
}
