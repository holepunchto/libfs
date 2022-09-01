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
on_read_batch (uv_fs_t *req) {
  fs_read_batch_t *read_req = (fs_read_batch_t *) req->data;

  read_req->remaining--;

  if (req->result < 0) {
    read_req->cb(read_req, req->result, -1);
  } else {
    read_req->len += req->result;

    if (read_req->remaining == 0) {
      read_req->cb(read_req, 0, read_req->len);
    }
  }

  uv_fs_req_cleanup(req);

  if (read_req->remaining) {
    read_req->bufs++;
    read_req->offsets++;

    int err = uv_fs_read(req->loop, req, req->file, read_req->bufs, 1, read_req->offsets[0], on_read_batch);

    if (err < 0) {
      read_req->cb(read_req, err, -1);
    }
  }
}

int
fs_read_batch (uv_loop_t *loop, fs_read_batch_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[], fs_read_batch_cb cb) {
  if (bufs_len < 1) return UV_EINVAL;

  req->req.data = req;
  req->file = file;
  req->bufs = bufs;
  req->offsets = offsets;
  req->remaining = bufs_len;
  req->len = 0;
  req->cb = cb;

  return uv_fs_read(loop, &req->req, file, bufs, 1, offsets[0], on_read_batch);
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
on_write_batch (uv_fs_t *req) {
  fs_write_batch_t *write_req = (fs_write_batch_t *) req->data;

  write_req->remaining--;

  if (req->result < 0) {
    write_req->cb(write_req, req->result, -1);
  } else {
    write_req->len += req->result;

    if (write_req->remaining == 0) {
      write_req->cb(write_req, 0, write_req->len);
    }
  }

  uv_fs_req_cleanup(req);

  if (write_req->remaining) {
    write_req->bufs++;
    write_req->offsets++;

    int err = uv_fs_write(req->loop, req, req->file, write_req->bufs, 1, write_req->offsets[0], on_write_batch);

    if (err < 0) {
      write_req->cb(write_req, err, -1);
    }
  }
}

int
fs_write_batch (uv_loop_t *loop, fs_write_batch_t *req, uv_file file, const uv_buf_t bufs[], size_t bufs_len, const int64_t offsets[], fs_write_batch_cb cb) {
  if (bufs_len < 1) return UV_EINVAL;

  req->req.data = req;
  req->file = file;
  req->bufs = bufs;
  req->offsets = offsets;
  req->remaining = bufs_len;
  req->len = 0;
  req->cb = cb;

  return uv_fs_write(loop, &req->req, file, bufs, 1, offsets[0], on_write_batch);
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
on_chmod (uv_fs_t *req) {
  fs_chmod_t *chmod_req = (fs_chmod_t *) req->data;

  if (req->result < 0) {
    chmod_req->cb(chmod_req, req->result);
  } else {
    chmod_req->cb(chmod_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_chmod (uv_loop_t *loop, fs_chmod_t *req, uv_file file, int mode, fs_chmod_cb cb) {
  req->req.data = req;
  req->file = file;
  req->mode = mode;
  req->cb = cb;

  return uv_fs_fchmod(loop, &req->req, file, mode, on_chmod);
}

static void
on_mkdir (uv_fs_t *req) {
  fs_mkdir_t *mkdir_req = (fs_mkdir_t *) req->data;

  if (req->result < 0) {
    mkdir_req->cb(mkdir_req, req->result);
  } else {
    mkdir_req->cb(mkdir_req, 0);
  }

  uv_fs_req_cleanup(req);
}

int
fs_mkdir (uv_loop_t *loop, fs_mkdir_t *req, const char *path, int mode, fs_mkdir_cb cb) {
  req->req.data = req;
  req->path = path;
  req->mode = mode;
  req->cb = cb;

  return uv_fs_mkdir(loop, &req->req, path, mode, on_mkdir);
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
