#include <uv.h>

#include "../include/fs.h"

static void
on_read (uv_fs_t *req) {
  fs_read_t *read_req = (fs_read_t *) req->data;

  if (req->result < 0) {
    read_req->cb(read_req, req->result, -1);
  } else {
    read_req->cb(read_req, 0, req->result);
  }

  uv_fs_req_cleanup(req);
}

int
fs_read (uv_loop_t *loop, fs_read_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_read_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_read(loop, &req->req, file, bufs, bufs_len, offset, on_read);
}
