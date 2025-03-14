#include <stddef.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

static void
fs__trim_work(uv_work_t *req) {
  fs_trim_t *r = (fs_trim_t *) req->data;

  r->result = fs__trim(r->file, r->offset, r->length);
}

static void
fs__trim_after_work(uv_work_t *req, int status) {
  fs_trim_t *r = (fs_trim_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fs_trim(uv_loop_t *loop, fs_trim_t *req, uv_file file, int64_t offset, size_t length, fs_trim_cb cb) {
  req->file = file;
  req->offset = offset;
  req->length = length;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__trim_work, fs__trim_after_work);
}
