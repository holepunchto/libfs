#include <limits.h>
#include <string.h>
#include <uv.h>

#include "../include/fs.h"

static void
on_realpath (uv_fs_t *req) {
  fs_realpath_t *realpath_req = (fs_realpath_t *) req->data;

  int status = req->result;

  char realpath[PATH_MAX];

  if (status >= 0) strncpy(realpath, (char *) req->ptr, PATH_MAX);

  uv_fs_req_cleanup(req);

  if (status < 0) {
    realpath_req->cb(realpath_req, status, NULL);
  } else {
    realpath_req->cb(realpath_req, 0, realpath);
  }
}

int
fs_realpath (uv_loop_t *loop, fs_realpath_t *req, const char *path, fs_realpath_cb cb) {
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_fs_realpath(loop, &req->req, path, on_realpath);
}
