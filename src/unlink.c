#include <uv.h>

#include "../include/fs.h"

static void
on_unlink (uv_fs_t *req) {
  fs_unlink_t *unlink_req = (fs_unlink_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    unlink_req->cb(unlink_req, status);
  } else {
    unlink_req->cb(unlink_req, 0);
  }
}

int
fs_unlink (uv_loop_t *loop, fs_unlink_t *req, const char *path, fs_unlink_cb cb) {
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_unlink(loop, &req->req, path, on_unlink);
}
