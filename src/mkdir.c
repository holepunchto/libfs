#include <assert.h>
#include <path.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "../include/fs.h"

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) &S_IFMT) == S_IFDIR)
#endif

typedef struct fs_mkdir_step_s fs_mkdir_step_t;

struct fs_mkdir_step_s {
  fs_mkdir_t *req;
  char *next;
};

static void
on_finished (fs_mkdir_t *req, int err) {
  uv_fs_req_cleanup(&req->req);

  free(req->path);

  if (req->cb) req->cb(req, err < 0 ? err : 0);
}

static void
on_stat (uv_fs_t *req) {
  fs_mkdir_step_t *rec = (fs_mkdir_step_t *) req->data;

  int err = req->result;

  if (err == 0 && !S_ISDIR(req->statbuf.st_mode)) {
    err = UV_ENOTDIR;
  }

  on_finished(rec->req, err);

  if (rec->next != NULL) free(rec->next);

  free(rec);
}

static void
on_mkdir_recursive (uv_fs_t *req) {
  fs_mkdir_step_t *rec = (fs_mkdir_step_t *) req->data;

  int err = req->result;

  switch (err) {
  case UV_EACCES:
  case UV_ENOSPC:
  case UV_ENOTDIR:
  case UV_EPERM:
    break;

  case 0: {
    if (rec->next == NULL) break;

    size_t len = strlen(rec->next);

    if (len == strlen(rec->req->path)) break;

    rec->next[len] = '/';

    uv_fs_req_cleanup(req);

    err = uv_fs_mkdir(req->loop, req, rec->next, rec->req->mode, on_mkdir_recursive);
    assert(err == 0);

    return;
  }

  case UV_ENOENT: {
    size_t dirname = 0;

    path_dirname(req->path, &dirname, path_behavior_system);

    if (dirname == strlen(req->path)) break;

    if (rec->next == NULL) {
      rec->next = strdup(rec->req->path);
    }

    rec->next[dirname - 1] = '\0';

    uv_fs_req_cleanup(req);

    err = uv_fs_mkdir(req->loop, req, rec->next, rec->req->mode, on_mkdir_recursive);
    assert(err == 0);

    return;
  }

  default:
    uv_fs_req_cleanup(req);

    err = uv_fs_stat(req->loop, req, rec->req->path, on_stat);
    assert(err == 0);

    return;
  }

  on_finished(rec->req, err);

  if (rec->next != NULL) free(rec->next);

  free(rec);
}

static void
on_mkdir (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  int err = req->result;

  uv_fs_req_cleanup(req);

  mkdir_req->cb(mkdir_req, err < 0 ? err : 0);
}

int
fs_mkdir (uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, bool recursive, fs_mkdir_cb cb) {
  req->path = strdup(path);
  req->mode = mode;
  req->cb = cb;

  if (recursive) {
    fs_mkdir_step_t *rec = malloc(sizeof(fs_mkdir_step_t));

    rec->req = req;
    rec->next = NULL;

    req->req.data = (void *) rec;
  } else {
    req->req.data = (void *) req;
  }

  return uv_fs_mkdir(loop, &req->req, path, mode, recursive ? on_mkdir_recursive : on_mkdir);
}
