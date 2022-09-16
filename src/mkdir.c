#include <uv.h>

#include "../include/fs.h"

#ifdef __POSIX__
#define FS_PATH_SEP "/"
#else
#define FS_PATH_SEP "/\\"
#endif

static void
on_mkdir (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  int err = req->result;

  if (err < 0) {
    if (mkdir_req->recursive && err == UV_EEXIST) {
      err = 0;
    }

    mkdir_req->cb(mkdir_req, err);
  } else {
    mkdir_req->cb(mkdir_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_mkdir (uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, bool recursive, fs_mkdir_cb cb) {
  req->req.data = req;
  req->path = path;
  req->dir = NULL;
  req->mode = mode;
  req->recursive = recursive;
  req->cb = cb;

  return uv_fs_mkdir(loop, &req->req, path, mode, on_mkdir);
}
