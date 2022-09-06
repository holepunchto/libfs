#include <uv.h>

#include "../include/fs.h"

static size_t
contiguous_bufs (const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[]) {
  size_t n = 1;

  while (n < bufs_len && offsets[n - 1] + bufs[n - 1].len == (uint64_t) offsets[n]) {
    n++;
  }

  return n;
}

static void
on_read_batch (uv_fs_t *req) {
  fs_read_batch_t *read_req = (fs_read_batch_t *) req->data;

  size_t batched = read_req->batched;

  read_req->remaining -= batched;

  if (req->result < 0) {
    read_req->cb(read_req, req->result, -1);
  } else {
    read_req->len += req->result;

    if (read_req->remaining == 0) {
      read_req->cb(read_req, 0, read_req->len);
    }
  }

  uv_fs_req_cleanup(req);

  if (read_req->remaining) {
    read_req->bufs += batched;
    read_req->offsets += batched;

    read_req->batched = contiguous_bufs(
      read_req->bufs,
      read_req->remaining,
      read_req->offsets
    );

    int err = uv_fs_read(
      req->loop,
      req,
      req->file,
      read_req->bufs,
      read_req->batched,
      read_req->offsets[0],
      on_read_batch
    );

    if (err < 0) {
      read_req->cb(read_req, err, -1);
    }
  }
}

int
fs_read_batch (uv_loop_t *loop, fs_read_batch_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[], fs_read_batch_cb cb) {
  if (bufs_len < 1) return UV_EINVAL;

  req->req.data = req;
  req->file = file;
  req->bufs = bufs;
  req->offsets = offsets;
  req->remaining = bufs_len;
  req->batched = contiguous_bufs(bufs, bufs_len, offsets);
  req->len = 0;
  req->cb = cb;

  return uv_fs_read(loop, &req->req, file, bufs, req->batched, offsets[0], on_read_batch);
}
