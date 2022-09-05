#include <uv.h>

#include "../include/fs.h"

static void
on_close (uv_fs_t *req) {
  fs_close_t *close_req = (fs_close_t *) req->data;

  if (req->result < 0) {
    close_req->cb(close_req, req->result);
  } else {
    close_req->cb(close_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_close (uv_loop_t *loop, fs_close_t *req, uv_file file, fs_close_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_close(loop, &req->req, file, on_close);
}
