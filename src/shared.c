#include <uv.h>

#include "../include/fs.h"

static void
on_open (uv_fs_t *req) {
  fs_open_t *open_req = (fs_open_t *) req->data;

  if (req->result < 0) {
    open_req->cb(open_req, req->result, -1);
  } else {
    open_req->cb(open_req, 0, req->result);
  }

  uv_fs_req_cleanup(req);
}

int
fs_open (uv_loop_t *loop, fs_open_t *req, const char *path, int flags, int mode, fs_open_cb cb) {
  req->req.data = req;
  req->path = path;
  req->cb = cb;

  return uv_fs_open(loop, &req->req, path, flags, mode, on_open);
}

static void
on_close (uv_fs_t *req) {
  fs_close_t *close_req = (fs_close_t *) req->data;

  if (req->result < 0) {
    close_req->cb(close_req, req->result);
  } else {
    close_req->cb(close_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_close (uv_loop_t *loop, fs_close_t *req, uv_file file, fs_close_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_close(loop, &req->req, file, on_close);
}

static void
on_read (uv_fs_t *req) {
  fs_read_t *read_req = (fs_read_t *) req->data;

  if (req->result < 0) {
    read_req->cb(read_req, req->result, -1);
  } else {
    read_req->cb(read_req, 0, req->result);
  }

  uv_fs_req_cleanup(req);
}

int
fs_read (uv_loop_t *loop, fs_read_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_read_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_read(loop, &req->req, file, bufs, bufs_len, offset, on_read);
}

static void
on_write (uv_fs_t *req) {
  fs_write_t *write_req = (fs_write_t *) req->data;

  if (req->result < 0) {
    write_req->cb(write_req, req->result, -1);
  } else {
    write_req->cb(write_req, 0, req->result);
  }

  uv_fs_req_cleanup(req);
}

int
fs_write (uv_loop_t *loop, fs_write_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, int64_t offset, fs_write_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_write(loop, &req->req, file, bufs, bufs_len, offset, on_write);
}

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

static void
on_truncate (uv_fs_t *req) {
  fs_truncate_t *truncate_req = (fs_truncate_t *) req->data;

  if (req->result < 0) {
    truncate_req->cb(truncate_req, req->result);
  } else {
    truncate_req->cb(truncate_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_truncate (uv_loop_t *loop, fs_truncate_t *req, uv_file file, int64_t offset, fs_truncate_cb cb) {
  req->req.data = req;
  req->file = file;
  req->cb = cb;

  return uv_fs_ftruncate(loop, &req->req, file, offset, on_truncate);
}

static void
on_unlink (uv_fs_t *req) {
  fs_unlink_t *unlink_req = (fs_unlink_t *) req->data;

  if (req->result < 0) {
    unlink_req->cb(unlink_req, req->result);
  } else {
    unlink_req->cb(unlink_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_unlink (uv_loop_t *loop, fs_unlink_t *req, const char *path, fs_unlink_cb cb) {
  req->req.data = req;
  req->path = path;
  req->cb = cb;

  return uv_fs_unlink(loop, &req->req, path, on_unlink);
}
