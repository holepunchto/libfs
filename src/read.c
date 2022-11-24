#include <uv.h>

#include "../include/fs.h"

static void
on_read (uv_fs_t *req) {
  fs_read_t *read_req = (fs_read_t *) req->data;

  ssize_t read = req->result;

  uv_fs_req_cleanup(req);

  if (read < 0) {
    read_req->cb(read_req, read, -1);
  } else {
    read_req->cb(read_req, 0, read);
  }
}

int
fs_read (uv_loop_t *loop, fs_read_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_read_cb cb) {
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_read(loop, &req->req, file, bufs, bufs_len, offset, on_read);
}
