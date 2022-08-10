#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_open_t open_req;
fs_stat_t stat_req;
fs_close_t close_req;

bool stat_called = false;

void
on_close (fs_close_t *req, int status) {
  assert(status == 0);
}

void
on_stat (fs_stat_t *req, int status, const uv_stat_t *stat) {
  assert(status == 0);
  assert(stat->st_size == 12);

  int e = fs_close(loop, &close_req, req->file, on_close);
  assert(e == 0);

  stat_called = true;
}

void
on_open (fs_open_t *req, int status, uv_file file) {
  assert(status == 0);

  int e = fs_stat(loop, &stat_req, file, on_stat);
  assert(e == 0);
}

int
main () {
  loop = uv_default_loop();

  fs_open(loop, &open_req, "test/fixtures/hello.txt", O_RDONLY, 0, on_open);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(stat_called);

  return 0;
}
