#include <uv.h>

#include "../include/fs.h"

static void
on_rename (uv_fs_t *req) {
  fs_rename_t *rename_req = (fs_rename_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    rename_req->cb(rename_req, status);
  } else {
    rename_req->cb(rename_req, 0);
  }
}

int
fs_rename (uv_loop_t *loop, fs_rename_t *req, const char *from, const char *to, fs_rename_cb cb) {
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_rename(loop, &req->req, from, to, on_rename);
}
