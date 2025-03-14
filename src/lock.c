#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

static void
fs__lock_work(uv_work_t *req) {
  fs_lock_t *r = (fs_lock_t *) req->data;

  r->result = fs__lock(r->file, r->offset, r->length, r->shared);
}

static void
fs__lock_after_work(uv_work_t *req, int status) {
  fs_lock_t *r = (fs_lock_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fs_lock(uv_loop_t *loop, fs_lock_t *req, uv_file file, int64_t offset, size_t length, bool shared, fs_lock_cb cb) {
  req->file = file;
  req->offset = offset;
  req->length = length;
  req->shared = shared;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__lock_work, fs__lock_after_work);
}

static void
fs__downgrade_lock_work(uv_work_t *req) {
  fs_lock_t *r = (fs_lock_t *) req->data;

  r->result = fs__downgrade_lock(r->file, r->offset, r->length);
}

int
fs_downgrade_lock(uv_loop_t *loop, fs_lock_t *req, uv_file file, int64_t offset, size_t length, fs_lock_cb cb) {
  req->file = file;
  req->offset = offset;
  req->length = length;
  req->shared = true;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__downgrade_lock_work, fs__lock_after_work);
}

static void
fs__upgrade_lock_work(uv_work_t *req) {
  fs_lock_t *r = (fs_lock_t *) req->data;

  r->result = fs__upgrade_lock(r->file, r->offset, r->length);
}

int
fs_upgrade_lock(uv_loop_t *loop, fs_lock_t *req, uv_file file, int64_t offset, size_t length, fs_lock_cb cb) {
  req->file = file;
  req->offset = offset;
  req->length = length;
  req->shared = false;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__upgrade_lock_work, fs__lock_after_work);
}
