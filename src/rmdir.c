#include <assert.h>
#include <path.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "../include/fs.h"

typedef struct fs_rmdir_step_s fs_rmdir_step_t;

struct fs_rmdir_step_s {
  fs_rmdir_t *req;
  fs_rmdir_step_t *parent;
  char *path;
  int missing;
};

static void
on_rmdir_recursive(uv_fs_t *req);

static void
on_finished(fs_rmdir_t *req) {
  uv_fs_req_cleanup(&req->req);

  free(req->path);

  if (req->cb) req->cb(req, req->result);
}

static void
on_finished_maybe(uv_fs_t *uv_req, fs_rmdir_step_t *rec, int err) {
  uv_fs_req_cleanup(uv_req);

  fs_rmdir_step_t *parent = rec->parent;
  fs_rmdir_t *req = rec->req;

  if (err < 0) req->result = err;

  if (rec->missing == 0) {
    free(rec->path);
    free(rec);

    if (parent == NULL) {
      on_finished(req);
    } else {
      parent->missing--;

      if (parent->missing == 0) {
        uv_req->data = parent;

        err = uv_fs_rmdir(uv_req->loop, uv_req, parent->path, on_rmdir_recursive);
        assert(err == 0);

        return;
      }
    }
  }

  if (parent) free(uv_req);
}

static void
on_unlink(uv_fs_t *req) {
  fs_rmdir_step_t *rec = (fs_rmdir_step_t *) req->data;

  int err = req->result;

  switch (err) {
  case UV_EPERM:
  case UV_EISDIR: {
    char *path = strdup(req->path);

    uv_fs_req_cleanup(req);

    fs_rmdir_step_t *child = malloc(sizeof(fs_rmdir_step_t));

    child->req = rec->req;
    child->parent = rec;
    child->path = path;
    child->missing = 0;

    req->data = child;

    err = uv_fs_rmdir(req->loop, req, path, on_rmdir_recursive);
    assert(err == 0);

    return;
  }

  default:
    rec->missing--;
  }

  on_finished_maybe(req, rec, err);
}

static void
on_scandir(uv_fs_t *req) {
  fs_rmdir_step_t *rec = (fs_rmdir_step_t *) req->data;

  ssize_t len = req->result;

  if (len <= 0) {
    on_finished_maybe(req, rec, len);

    return;
  }

  uv_dirent_t entry;

  char path[4097];
  size_t path_len;

  for (size_t i = 0, n = (size_t) len; i < n; i++) {
    int err;

    uv_fs_scandir_next(req, &entry);

    path_len = 4097;

    err = path_join(
      (const char *[]) {req->path, entry.name, NULL},
      path,
      &path_len,
      path_behavior_system
    );

    if (err < 0) continue;

    uv_fs_t *rm_req = malloc(sizeof(uv_fs_t));

    rm_req->data = (void *) rec;

    err = uv_fs_unlink(req->loop, rm_req, path, on_unlink);
    assert(err == 0);

    rec->missing++;
  }

  uv_fs_req_cleanup(req);
}

static void
on_rmdir_recursive(uv_fs_t *req) {
  fs_rmdir_step_t *rec = (fs_rmdir_step_t *) req->data;

  int err = req->result;

  uv_fs_req_cleanup(req);

  switch (err) {
  case 0:
    break;

  case UV_ENOTEMPTY: {
    err = uv_fs_scandir(req->loop, req, rec->path, 0, on_scandir);
    assert(err == 0);

    return;
  }
  }

  on_finished_maybe(req, rec, err);
}

static void
on_rmdir(uv_fs_t *req) {
  fs_rmdir_t *rmdir_req = (fs_rmdir_t *) req->data;

  int err = req->result;

  if (err < 0) rmdir_req->result = err;

  on_finished(rmdir_req);
}

int
fs_rmdir(uv_loop_t *loop, fs_rmdir_t *req, const char *path, bool recursive, fs_rmdir_cb cb) {
  req->path = strdup(path);
  req->cb = cb;
  req->result = 0;

  if (recursive) {
    fs_rmdir_step_t *rec = malloc(sizeof(fs_rmdir_step_t));

    rec->req = req;
    rec->parent = NULL;
    rec->path = strdup(path);
    rec->missing = 0;

    req->req.data = (void *) rec;
  } else {
    req->req.data = (void *) req;
  }

  return uv_fs_rmdir(loop, &req->req, path, recursive ? on_rmdir_recursive : on_rmdir);
}
