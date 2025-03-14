#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_symlink_t symlink_req;
fs_unlink_t unlink_req;

bool symlink_called = false;

void
on_symlink(fs_symlink_t *req, int status) {
  assert(status == 0);

  symlink_called = true;
}

void
on_unlink(fs_unlink_t *req, int status) {
  fs_symlink(loop, &symlink_req, "read.txt", "test/fixtures/read-link.txt", 0, on_symlink);
}

int
main() {
  loop = uv_default_loop();

  fs_unlink(loop, &unlink_req, "test/fixtures/read-link.txt", on_unlink);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(symlink_called);

  return 0;
}
