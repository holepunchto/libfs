#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_open_t open_req;
fs_read_t read_req;
fs_close_t close_req;

uv_buf_t buf;

bool read_called = false;

void
on_close (fs_close_t *req, int status) {
  assert(status == 0);
}

void
on_read (fs_read_t *req, int status, size_t len) {
  assert(status == 0);
  assert(len == 12);
  assert(memcmp(buf.base, "hello world\n", len) == 0);

  int e = fs_close(loop, &close_req, req->file, on_close);
  assert(e == 0);

  read_called = true;
}

void
on_open (fs_open_t *req, int status, uv_file file) {
  assert(status == 0);

  int e = fs_read(loop, &read_req, file, &buf, 1, 0, on_read);
  assert(e == 0);
}

int
main () {
  loop = uv_default_loop();

  buf = uv_buf_init(malloc(12), 12);

  fs_open(loop, &open_req, "test/fixtures/read.txt", O_RDONLY, 0, on_open);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(read_called);

  free(buf.base);

  return 0;
}
