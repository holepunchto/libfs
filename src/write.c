#include <uv.h>

#include "../include/fs.h"

static void
on_write (uv_fs_t *req) {
  fs_write_t *write_req = (fs_write_t *) req->data;

  ssize_t written = req->result;

  uv_fs_req_cleanup(req);

  if (written < 0) {
    write_req->cb(write_req, written, 0);
  } else {
    write_req->cb(write_req, 0, written);
  }
}

int
fs_write (uv_loop_t *loop, fs_write_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_write_cb cb) {
  req->file = file;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_write(loop, &req->req, file, bufs, bufs_len, offset, on_write);
}
