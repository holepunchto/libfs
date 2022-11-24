#include <uv.h>

#include "../include/fs.h"

static void
on_access (uv_fs_t *req) {
  fs_access_t *access_req = (fs_access_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    access_req->cb(access_req, status);
  } else {
    access_req->cb(access_req, 0);
  }
}

int
fs_access (uv_loop_t *loop, fs_access_t *req, const char *path, int mode, fs_access_cb cb) {
  req->mode = mode;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_access(loop, &req->req, path, mode, on_access);
}
