#include <uv.h>

#include "../include/fs.h"

static void
on_chmod (uv_fs_t *req) {
  fs_chmod_t *chmod_req = (fs_chmod_t *) req->data;

  if (req->result < 0) {
    chmod_req->cb(chmod_req, req->result);
  } else {
    chmod_req->cb(chmod_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_chmod (uv_loop_t *loop, fs_chmod_t *req, uv_file file, int mode, fs_chmod_cb cb) {
  req->req.data = req;
  req->file = file;
  req->mode = mode;
  req->cb = cb;

  return uv_fs_fchmod(loop, &req->req, file, mode, on_chmod);
}