#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <uv.h>

#include "../include/fs.h"

uv_loop_t *loop;

fs_open_t open_req;
fs_list_attrs_t list_req;
fs_set_attr_t set_req;

uv_buf_t buf;
uv_file file;

bool list_called = false;

void
on_list (fs_list_attrs_t *req, int status, const char *names[], ssize_t length) {
  assert(status == 0);

  list_called = true;

  assert(length == 1);
  assert(strcmp(names[0], "test") == 0);
}

void
on_set (fs_set_attr_t *req, int status) {
  assert(status == 0);

  int e = fs_list_attrs(loop, &list_req, file, on_list);
  assert(e == 0);
}

void
on_open (fs_open_t *req, int status, uv_file fd) {
  assert(status == 0);

  buf.base = "hello world";
  buf.len = 11;

  file = fd;

  int e = fs_set_attr(loop, &set_req, file, "test", &buf, on_set);
  assert(e == 0);
}

int
main () {
  loop = uv_default_loop();

  int e = fs_open(loop, &open_req, "test/fixtures/read.txt", O_RDWR, 0, on_open);
  assert(e == 0);

  uv_run(loop, UV_RUN_DEFAULT);

  assert(list_called);

  return 0;
}
