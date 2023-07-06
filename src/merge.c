#include <assert.h>
#include <path.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "../include/fs.h"

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) &S_IFMT) == S_IFDIR)
#endif

typedef struct fs_merge_step_s fs_merge_step_t;

struct fs_merge_step_s {
  fs_merge_t *req;
  fs_merge_step_t *parent;
  char *base;
  char *onto;
  int missing;
};

static void
on_rmdir (uv_fs_t *req);

static void
on_finished (fs_merge_t *req) {
  uv_fs_req_cleanup(&req->req);

  if (req->cb) req->cb(req, req->result);
}

static void
on_finished_maybe (uv_fs_t *uv_req, fs_merge_step_t *rec, int err) {
  uv_fs_req_cleanup(uv_req);

  fs_merge_step_t *parent = rec->parent;
  fs_merge_t *req = rec->req;

  if (err < 0) req->result = err;

  if (rec->missing == 0) {
    free(rec->base);
    free(rec->onto);
    free(rec);

    if (parent == NULL) {
      on_finished(req);
    } else {
      parent->missing--;

      if (parent->missing == 0) {
        uv_req->data = parent;

        err = uv_fs_rmdir(uv_req->loop, uv_req, parent->base, on_rmdir);
        assert(err == 0);

        return;
      }
    }
  }

  if (parent) free(uv_req);
}

static void
on_rmdir (uv_fs_t *req) {
  fs_merge_step_t *rec = (fs_merge_step_t *) req->data;

  int err = req->result;

  on_finished_maybe(req, rec, err);
}

static void
on_rename (uv_fs_t *req) {
  fs_merge_step_t *rec = (fs_merge_step_t *) req->data;

  int err = req->result;

  on_finished_maybe(req, rec, err);
}

static void
on_stat_file (uv_fs_t *req);

static void
on_stat_dir (uv_fs_t *req);

static void
on_scandir (uv_fs_t *req) {
  fs_merge_step_t *rec = (fs_merge_step_t *) req->data;

  ssize_t len = req->result;

  if (len < 0) return on_finished_maybe(req, rec, len);

  if (len == 0) {
    uv_fs_req_cleanup(req);

    int err = uv_fs_rmdir(req->loop, req, rec->base, on_rmdir);
    assert(err == 0);

    return;
  }

  uv_dirent_t entry;

  char base[4097], onto[4097];
  size_t path_len;

  for (size_t i = 0, n = (size_t) len; i < n; i++) {
    int err;

    uv_fs_scandir_next(req, &entry);

    path_len = 4097;

    err = path_join(
      (const char *[]){rec->base, entry.name, NULL},
      base,
      &path_len,
      path_behavior_system
    );

    if (err < 0) continue;

    path_len = 4097;

    err = path_join(
      (const char *[]){rec->onto, entry.name, NULL},
      onto,
      &path_len,
      path_behavior_system
    );

    if (err < 0) continue;

    fs_merge_step_t *child = malloc(sizeof(fs_merge_step_t));

    child->req = rec->req;
    child->parent = rec;
    child->base = strdup(base);
    child->onto = strdup(onto);
    child->missing = 0;

    uv_fs_t *stat_req = malloc(sizeof(uv_fs_t));

    stat_req->data = (void *) child;

    err = uv_fs_stat(req->loop, stat_req, onto, entry.type == UV_DIRENT_DIR ? on_stat_dir : on_stat_file);
    assert(err == 0);

    rec->missing++;
  }

  uv_fs_req_cleanup(req);
}

static void
on_stat_file (uv_fs_t *req) {
  fs_merge_step_t *rec = (fs_merge_step_t *) req->data;

  int err = req->result;

  if (err == 0 && S_ISDIR(req->statbuf.st_mode)) {
    err = UV_EISDIR;
  }

  uv_fs_req_cleanup(req);

  switch (err) {
  case 0: {
    if (rec->req->replace) {
      // Fallthrough
    } else {
      err = UV_EEXIST;

      break;
    }
  }

  case UV_ENOENT: {
    err = uv_fs_rename(req->loop, req, rec->base, rec->onto, on_rename);
    assert(err == 0);

    return;
  }
  }

  on_finished_maybe(req, rec, err);
}

static void
on_stat_dir (uv_fs_t *req) {
  fs_merge_step_t *rec = (fs_merge_step_t *) req->data;

  int err = req->result;

  if (err == 0 && !S_ISDIR(req->statbuf.st_mode)) {
    err = UV_ENOTDIR;
  }

  uv_fs_req_cleanup(req);

  switch (err) {
  case 0: {
    err = uv_fs_scandir(req->loop, req, rec->base, 0, on_scandir);
    assert(err == 0);

    return;
  }

  case UV_ENOENT: {
    err = uv_fs_rename(req->loop, req, rec->base, rec->onto, on_rename);
    assert(err == 0);

    return;
  }
  }

  on_finished_maybe(req, rec, err);
}

int
fs_merge (uv_loop_t *loop, fs_merge_t *req, const char *base, const char *onto, bool replace, fs_merge_cb cb) {
  req->cb = cb;
  req->replace = replace;
  req->result = 0;

  fs_merge_step_t *rec = malloc(sizeof(fs_merge_step_t));

  rec->req = req;
  rec->parent = NULL;
  rec->base = strdup(base);
  rec->onto = strdup(onto);
  rec->missing = 0;

  req->req.data = (void *) rec;

  return uv_fs_lstat(loop, &req->req, onto, on_stat_dir);
}
