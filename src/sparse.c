#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

static void
fs__sparse_work (uv_work_t *req) {
  fs_sparse_t *r = (fs_sparse_t *) req->data;

  r->result = fs__sparse(r->file);
}

static void
fs__sparse_after_work (uv_work_t *req, int status) {
  fs_sparse_t *r = (fs_sparse_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fs_sparse (uv_loop_t *loop, fs_sparse_t *req, uv_file file, fs_sparse_cb cb) {
  req->file = file;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__sparse_work, fs__sparse_after_work);
}
