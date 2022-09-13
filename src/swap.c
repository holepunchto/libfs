#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

static void
fs__swap_work (uv_work_t *req) {
  fs_swap_t *r = (fs_swap_t *) req->data;

  r->result = fs__swap(r->from, r->to);
}

static void
fs__swap_after_work (uv_work_t *req, int status) {
  fs_swap_t *r = (fs_swap_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fs_swap (uv_loop_t *loop, fs_swap_t *req, const char *from, const char *to, fs_swap_cb cb) {
  req->from = from;
  req->to = to;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__swap_work, fs__swap_after_work);
}
