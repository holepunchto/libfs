#include <uv.h>

#include "../include/fs.h"

static void
on_truncate (uv_fs_t *req) {
  fs_truncate_t *truncate_req = (fs_truncate_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    truncate_req->cb(truncate_req, status);
  } else {
    truncate_req->cb(truncate_req, 0);
  }
}

int
fs_truncate (uv_loop_t *loop, fs_truncate_t *req, uv_file file, int64_t offset, fs_truncate_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_ftruncate(loop, &req->req, file, offset, on_truncate);
}
