#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_open_t open_req;
fs_read_batch_t read_req;
fs_close_t close_req;

uv_buf_t bufs[2];
int64_t offsets[2];

bool read_called = false;

void
on_close (fs_close_t *req, int status) {
  assert(status == 0);
}

void
on_read (fs_read_batch_t *req, int status, ssize_t len) {
  assert(status == 0);
  assert(len == 6);
  assert(memcmp(bufs[0].base, "hel", 3) == 0);
  assert(memcmp(bufs[1].base, "wor", 3) == 0);

  int e = fs_close(loop, &close_req, req->file, on_close);
  assert(e == 0);

  read_called = true;
}

void
on_open (fs_open_t *req, int status, uv_file file) {
  assert(status == 0);

  int e = fs_read_batch(loop, &read_req, file, bufs, 2, offsets, on_read);
  assert(e == 0);
}

int
main () {
  loop = uv_default_loop();

  bufs[0] = uv_buf_init(malloc(3), 3);
  offsets[0] = 0;

  bufs[1] = uv_buf_init(malloc(3), 3);
  offsets[1] = 6;

  fs_open(loop, &open_req, "test/fixtures/read.txt", O_RDONLY, 0, on_open);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(read_called);

  free(bufs[0].base);
  free(bufs[1].base);

  return 0;
}
