#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_mkdir_t mkdir_req;
fs_merge_t merge_req;

bool merge_called = false;

void
on_mkdir (fs_mkdir_t *req, int status);

void
on_merge (fs_merge_t *req, int status);

void
generate_directories () {
  int err;

  static char *directories[] = {
    // Base
    "test/fixtures/merge/base/a/b/c",
    "test/fixtures/merge/base/a/d",
    "test/fixtures/merge/base/c",

    // Onto
    "test/fixtures/merge/onto/a/b/d",

    NULL,
  };

  static size_t i = 0;

  char *directory = directories[i++];

  if (directory) {
    err = fs_mkdir(loop, &mkdir_req, directory, 0777, true, on_mkdir);
    assert(err == 0);
  } else {
    err = fs_merge(loop, &merge_req, "test/fixtures/merge/base", "test/fixtures/merge/onto", true, on_merge);
    assert(err == 0);
  }
}

void
on_mkdir (fs_mkdir_t *req, int status) {
  assert(status == 0);

  generate_directories();
}

void
on_merge (fs_merge_t *req, int status) {
  assert(status == 0);

  merge_called = true;
}

int
main () {
  loop = uv_default_loop();

  generate_directories();

  uv_run(loop, UV_RUN_DEFAULT);

  assert(merge_called);

  return 0;
}
