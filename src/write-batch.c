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
on_write_batch (uv_fs_t *req) {
  fs_write_batch_t *write_req = (fs_write_batch_t *) req->data;

  size_t batched = write_req->batched;

  write_req->remaining -= batched;

  ssize_t written = req->result;

  uv_fs_req_cleanup(req);

  if (written < 0) {
    write_req->cb(write_req, written, 0);
  } else {
    write_req->len += written;

    if (write_req->remaining == 0) {
      write_req->cb(write_req, 0, write_req->len);
    }
  }

  if (write_req->remaining) {
    write_req->bufs += batched;
    write_req->offsets += batched;

    write_req->batched = contiguous_bufs(
      write_req->bufs,
      write_req->remaining,
      write_req->offsets
    );

    int err = uv_fs_read(
      req->loop,
      req,
      write_req->file,
      write_req->bufs,
      write_req->batched,
      write_req->offsets[0],
      on_write_batch
    );

    if (err < 0) {
      write_req->cb(write_req, err, -1);
    }
  }
}

int
fs_write_batch (uv_loop_t *loop, fs_write_batch_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[], fs_write_batch_cb cb) {
  if (bufs_len < 1) return UV_EINVAL;

  req->file = file;
  req->bufs = bufs;
  req->offsets = offsets;
  req->remaining = bufs_len;
  req->batched = contiguous_bufs(bufs, bufs_len, offsets);
  req->len = 0;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_write(loop, &req->req, file, bufs, req->batched, offsets[0], on_write_batch);
}
