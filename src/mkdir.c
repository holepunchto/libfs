#include <uv.h>

#include "../include/fs.h"

static void
on_mkdir (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  if (req->result < 0) {
    mkdir_req->cb(mkdir_req, req->result);
  } else {
    mkdir_req->cb(mkdir_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_mkdir (uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, fs_mkdir_cb cb) {
  req->req.data = req;
  req->path = path;
  req->mode = mode;
  req->cb = cb;

  return uv_fs_mkdir(loop, &req->req, path, mode, on_mkdir);
}
