#include <assert.h>
#include <stdbool.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_mkdir_t mkdir_req;
fs_rmdir_t rmdir_req;

bool mkdir_called = false;
bool rmdir_called = false;

void
on_rmdir (fs_rmdir_t *req, int status) {
  assert(status == 0);

  rmdir_called = true;
}

void
on_mkdir (fs_mkdir_t *req, int status) {
  assert(status == 0);

  mkdir_called = true;

  int e = fs_rmdir(loop, &rmdir_req, "test/fixtures/mkdir", true, on_rmdir);
  assert(e == 0);
}

int
main () {
  loop = uv_default_loop();

  int e = fs_mkdir(loop, &mkdir_req, "test/fixtures/mkdir/a/b/c", 0777, true, on_mkdir);
  assert(e == 0);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(mkdir_called);
  assert(rmdir_called);

  return 0;
}
