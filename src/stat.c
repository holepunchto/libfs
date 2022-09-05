#include <uv.h>

#include "../include/fs.h"

static void
on_stat (uv_fs_t *req) {
  fs_stat_t *stat_req = (fs_stat_t *) req->data;

  if (req->result < 0) {
    stat_req->cb(stat_req, req->result, NULL);
  } else {
    stat_req->cb(stat_req, 0, &req->statbuf);
  }

  uv_fs_req_cleanup(req);
}

int
fs_stat (uv_loop_t *loop, fs_stat_t *req, uv_file file, fs_stat_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_fstat(loop, &req->req, file, on_stat);
}
