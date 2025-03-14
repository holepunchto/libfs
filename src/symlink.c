#include <uv.h>

#include "../include/fs.h"

static void
on_symlink(uv_fs_t *req) {
  fs_symlink_t *symlink_req = (fs_symlink_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    symlink_req->cb(symlink_req, status);
  } else {
    symlink_req->cb(symlink_req, 0);
  }
}

int
fs_symlink(uv_loop_t *loop, fs_symlink_t *req, const char *target, const char *link, int flags, fs_symlink_cb cb) {
  req->flags = flags;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_symlink(loop, &req->req, target, link, flags, on_symlink);
}
