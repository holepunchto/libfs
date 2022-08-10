#include <assert.h>
#include <stdbool.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_open_t open_req;
fs_close_t close_req;

bool open_called = false;
bool close_called = false;

void
on_close (fs_close_t *req, int status) {
  assert(status == 0);

  close_called = true;
}

void
on_open (fs_open_t *req, int status, uv_file file) {
  assert(status == 0);

  int e = fs_close(loop, &close_req, file, on_close);
  assert(e == 0);

  open_called = true;
}

int
main () {
  loop = uv_default_loop();

  int e = fs_open(loop, &open_req, "test/fixtures/hello.txt", O_RDONLY, 0, on_open);
  assert(e == 0);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(open_called);
  assert(close_called);

  return 0;
}
