#include <uv.h>

#include "../include/fs.h"

static void
on_open (uv_fs_t *req) {
  fs_open_t *open_req = (fs_open_t *) req->data;

  if (req->result < 0) {
    open_req->cb(open_req, req->result, -1);
  } else {
    open_req->cb(open_req, 0, req->result);
  }

  uv_fs_req_cleanup(req);
}

int
fs_open (uv_loop_t *loop, fs_open_t *req, const char *path, int flags, int mode, fs_open_cb cb) {
  req->req.data = req;
  req->path = path;
  req->cb = cb;

  return uv_fs_open(loop, &req->req, path, flags, mode, on_open);
}
