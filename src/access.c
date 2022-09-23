#include <uv.h>

#include "../include/fs.h"

static void
on_access (uv_fs_t *req) {
  fs_access_t *access_req = (fs_access_t *) req->data;

  if (req->result < 0) {
    access_req->cb(access_req, req->result);
  } else {
    access_req->cb(access_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_access (uv_loop_t *loop, fs_access_t *req, const char *path, int mode, fs_access_cb cb) {
  req->req.data = req;
  req->path = path;
  req->mode = mode;
  req->cb = cb;

  return uv_fs_access(loop, &req->req, path, mode, on_access);
}
