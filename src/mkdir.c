#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) &S_IFMT) == S_IFDIR)
#endif

typedef struct fs_mkdir_recursive_s fs_mkdir_recursive_t;

struct fs_mkdir_recursive_s {
  fs_mkdir_t *req;
  char *next;
};

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
on_finished (fs_mkdir_recursive_t *rec, int err) {
  rec->req->cb(rec->req, err < 0 ? err : 0);

  if (rec->next != NULL) free(rec->next);

  uv_fs_req_cleanup(&rec->req->req);

  free(rec);
}

static void
on_stat (uv_fs_t *req) {
  fs_mkdir_recursive_t *rec = (fs_mkdir_recursive_t *) req->data;

  int err = req->result;

  if (err == 0 && !S_ISDIR(req->statbuf.st_mode)) {
    err = UV_ENOTDIR;
  }

  on_finished(rec, err);
}

static void
on_mkdir_recursive (uv_fs_t *req) {
  fs_mkdir_recursive_t *mkdir_req = (fs_mkdir_recursive_t *) req->data;

  int err = req->result;

  switch (err) {
  case UV_EACCES:
  case UV_ENOSPC:
  case UV_ENOTDIR:
  case UV_EPERM:
    break;

  case 0: {
    if (mkdir_req->next == NULL) break;

    size_t len = strlen(mkdir_req->next);

    if (len == strlen(mkdir_req->req->path)) break;

    mkdir_req->next[len] = '/';

    uv_fs_req_cleanup(req);

    err = uv_fs_mkdir(req->loop, req, mkdir_req->next, mkdir_req->req->mode, on_mkdir_recursive);

    if (err == 0) return;

    break;
  }

  case UV_ENOENT: {
    size_t n = last_path_sep(req->path);

    if (n == (size_t) -1) break;

    if (mkdir_req->next == NULL) {
      mkdir_req->next = strdup(mkdir_req->req->path);
    }

    mkdir_req->next[n] = '\0';

    uv_fs_req_cleanup(req);

    err = uv_fs_mkdir(req->loop, req, mkdir_req->next, mkdir_req->req->mode, on_mkdir_recursive);

    if (err == 0) return;

    break;
  }

  default:
    uv_fs_req_cleanup(req);

    err = uv_fs_stat(req->loop, req, mkdir_req->req->path, on_stat);

    if (err == 0) return;
  }

  on_finished(mkdir_req, err);
}

static void
on_mkdir (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  int err = req->result;

  mkdir_req->cb(mkdir_req, err < 0 ? err : 0);

  uv_fs_req_cleanup(req);
}

int
fs_mkdir (uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, bool recursive, fs_mkdir_cb cb) {
  req->path = path;
  req->mode = mode;
  req->cb = cb;

  if (recursive) {
    fs_mkdir_recursive_t *rec = malloc(sizeof(fs_mkdir_recursive_t));

    rec->req = req;
    rec->next = NULL;

    req->req.data = rec;
  } else {
    req->req.data = req;
  }

  return uv_fs_mkdir(loop, &req->req, path, mode, recursive ? on_mkdir_recursive : on_mkdir);
}
