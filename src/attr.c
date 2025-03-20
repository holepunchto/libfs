#include <stdlib.h>
#include <uv.h>

#include "../include/fs.h"
#include "platform.h"

static void
fs__get_attr_work(uv_work_t *req) {
  fs_get_attr_t *r = (fs_get_attr_t *) req->data;

  r->result = fs__get_attr(r->file, r->name, &r->value);
}

static void
fs__get_attr_after_work(uv_work_t *req, int status) {
  fs_get_attr_t *r = (fs_get_attr_t *) req->data;

  uv_buf_t value = r->value;

  if (r->cb) r->cb(r, r->result, &value);

  free(value.base);
}

int
fs_get_attr(uv_loop_t *loop, fs_get_attr_t *req, uv_file file, const char *name, fs_get_attr_cb cb) {
  req->file = file;
  req->name = name;
  req->value.base = NULL;
  req->value.len = 0;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__get_attr_work, fs__get_attr_after_work);
}

static void
fs__set_attr_work(uv_work_t *req) {
  fs_set_attr_t *r = (fs_set_attr_t *) req->data;

  r->result = fs__set_attr(r->file, r->name, &r->value);
}

static void
fs__set_attr_after_work(uv_work_t *req, int status) {
  fs_set_attr_t *r = (fs_set_attr_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fs_set_attr(uv_loop_t *loop, fs_set_attr_t *req, uv_file file, const char *name, const uv_buf_t *value, fs_set_attr_cb cb) {
  req->file = file;
  req->name = name;
  req->value = *value;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__set_attr_work, fs__set_attr_after_work);
}

static void
fs__remove_attr_work(uv_work_t *req) {
  fs_remove_attr_t *r = (fs_remove_attr_t *) req->data;

  r->result = fs__remove_attr(r->file, r->name);
}

static void
fs__remove_attr_after_work(uv_work_t *req, int status) {
  fs_remove_attr_t *r = (fs_remove_attr_t *) req->data;

  if (r->cb) r->cb(r, r->result);
}

int
fs_remove_attr(uv_loop_t *loop, fs_remove_attr_t *req, uv_file file, const char *name, fs_remove_attr_cb cb) {
  req->file = file;
  req->name = name;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__remove_attr_work, fs__remove_attr_after_work);
}

static void
fs__list_attrs_work(uv_work_t *req) {
  fs_list_attrs_t *r = (fs_list_attrs_t *) req->data;

  r->result = fs__list_attrs(r->file, &r->names, &r->length);
}

static void
fs__list_attrs_after_work(uv_work_t *req, int status) {
  fs_list_attrs_t *r = (fs_list_attrs_t *) req->data;

  char *names = r->names;

  if (r->cb) r->cb(r, r->result, (const char **) names, r->length);

  free(names);
}

int
fs_list_attrs(uv_loop_t *loop, fs_list_attrs_t *req, uv_file file, fs_list_attrs_cb cb) {
  req->file = file;
  req->cb = cb;
  req->req.data = (void *) req;

  return uv_queue_work(loop, &req->req, fs__list_attrs_work, fs__list_attrs_after_work);
}
