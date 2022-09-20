#include <uv.h>

#include "../include/fs.h"

static void
on_rmdir (uv_fs_t *req) {
  fs_rmdir_t *rmdir_req = (fs_rmdir_t *) req->data;

  if (req->result < 0) {
    rmdir_req->cb(rmdir_req, req->result);
  } else {
    rmdir_req->cb(rmdir_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_rmdir (uv_loop_t *loop, fs_rmdir_t *req, const char *path, bool recursive, fs_rmdir_cb cb) {
  req->req.data = req;
  req->path = path;
  req->recursive = recursive;
  req->cb = cb;

  return uv_fs_rmdir(loop, &req->req, path, on_rmdir);
}
