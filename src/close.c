#include <uv.h>

#include "../include/fs.h"

static void
on_close (uv_fs_t *req) {
  fs_close_t *close_req = (fs_close_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    close_req->cb(close_req, status);
  } else {
    close_req->cb(close_req, 0);
  }
}

int
fs_close (uv_loop_t *loop, fs_close_t *req, uv_file file, fs_close_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_close(loop, &req->req, file, on_close);
}
