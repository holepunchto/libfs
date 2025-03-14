#include <uv.h>

#include "../include/fs.h"

static void
on_stat(uv_fs_t *req) {
  fs_stat_t *stat_req = (fs_stat_t *) req->data;

  int status = req->result;

  uv_fs_req_cleanup(req);

  if (status < 0) {
    stat_req->cb(stat_req, status, NULL);
  } else {
    stat_req->cb(stat_req, 0, &req->statbuf);
  }
}

int
fs_stat(uv_loop_t *loop, fs_stat_t *req, uv_file file, fs_stat_cb cb) {
  req->file = file;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_fstat(loop, &req->req, file, on_stat);
}
