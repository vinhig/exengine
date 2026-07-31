#include "test.h"
#include <exengine/util/list.h>
#include <stdlib.h>

TEST(list_new) {
  ex_list_node_t *n = ex_list_new();
  CHECK_NOT_NULL(n);
  CHECK_NULL(n->data);
  CHECK_NULL(n->next);
  CHECK_NULL(n->prev);
  ex_list_destroy(n);
}

TEST(list_add_single) {
  ex_list_node_t *n = ex_list_new();
  int val = 42;
  ex_list_add(n, &val);
  CHECK(n->data == &val);
  CHECK_NOT_NULL(n->next);
  CHECK_NULL(n->next->data); /* sentinel node */
  ex_list_destroy(n);
}

TEST(list_add_multiple) {
  ex_list_node_t *n = ex_list_new();
  int a = 1, b = 2, c = 3;
  ex_list_add(n, &a);
  ex_list_add(n, &b);
  ex_list_add(n, &c);

  ex_list_node_t *cur = n;
  CHECK(cur->data == &a); cur = cur->next;
  CHECK(cur->data == &b); cur = cur->next;
  CHECK(cur->data == &c); cur = cur->next;
  CHECK_NULL(cur->data); /* sentinel */
  ex_list_destroy(n);
}

TEST(list_add_many) {
  ex_list_node_t *n = ex_list_new();
  int values[100];
  for (int i = 0; i < 100; i++) {
    values[i] = i;
    ex_list_add(n, &values[i]);
  }
  ex_list_node_t *cur = n;
  for (int i = 0; i < 100; i++) {
    CHECK(cur->data == &values[i]);
    cur = cur->next;
  }
  CHECK_NULL(cur->data); /* sentinel */
  ex_list_destroy(n);
}

TEST(list_remove_head) {
  ex_list_node_t *n = ex_list_new();
  int a = 1, b = 2;
  ex_list_add(n, &a);
  ex_list_add(n, &b);
  ex_list_t *r = ex_list_remove(n, &a);
  CHECK_NOT_NULL(r);
  CHECK(r->data == &b);
  ex_list_destroy(r);
}

TEST(list_remove_tail) {
  ex_list_node_t *n = ex_list_new();
  int a = 1, b = 2;
  ex_list_add(n, &a);
  ex_list_add(n, &b);
  ex_list_t *r = ex_list_remove(n, &b);
  CHECK(r->data == &a);
  CHECK_NULL(r->next->data); /* sentinel */
  ex_list_destroy(r);
}

TEST(list_remove_middle) {
  ex_list_node_t *n = ex_list_new();
  int a = 1, b = 2, c = 3;
  ex_list_add(n, &a);
  ex_list_add(n, &b);
  ex_list_add(n, &c);
  ex_list_t *r = ex_list_remove(n, &b);
  CHECK(r->data == &a);
  CHECK(r->next->data == &c);
  ex_list_destroy(r);
}

TEST(list_remove_last_element) {
  ex_list_node_t *n = ex_list_new();
  int a = 1;
  ex_list_add(n, &a);
  ex_list_t *r = ex_list_remove(n, &a);
  /* Removing the only element: r should be the sentinel, or NULL-like */
  CHECK_NOT_NULL(r);
  CHECK_NULL(r->data);
  ex_list_destroy(r);
}

TEST(list_remove_not_found) {
  ex_list_node_t *n = ex_list_new();
  int a = 1;
  ex_list_add(n, &a);
  int nonexistent = 99;
  ex_list_t *r = ex_list_remove(n, &nonexistent);
  CHECK(r == n); /* root unchanged */
  ex_list_destroy(r);
}

TEST(list_destroy_null) {
  ex_list_destroy(NULL); /* should not crash */
}

TEST(list_destroy_empty) {
  ex_list_node_t *n = ex_list_new();
  ex_list_destroy(n); /* should not crash */
}

TEST(list_prev_pointers) {
  ex_list_node_t *n = ex_list_new();
  int a = 1, b = 2, c = 3;
  ex_list_add(n, &a);
  ex_list_add(n, &b);
  ex_list_add(n, &c);
  ex_list_node_t *cur = n;
  CHECK_NULL(cur->prev);
  cur = cur->next;
  CHECK(cur->prev->data == &a);
  cur = cur->next;
  CHECK(cur->prev->data == &b);
  cur = cur->next;
  CHECK(cur->prev->data == &c); /* sentinel's prev points to c */
  ex_list_destroy(n);
}

int main(void) {
  TEST_RUN_INTERNAL(list_new);
  TEST_RUN_INTERNAL(list_add_single);
  TEST_RUN_INTERNAL(list_add_multiple);
  TEST_RUN_INTERNAL(list_add_many);
  TEST_RUN_INTERNAL(list_remove_head);
  TEST_RUN_INTERNAL(list_remove_tail);
  TEST_RUN_INTERNAL(list_remove_middle);
  TEST_RUN_INTERNAL(list_remove_last_element);
  TEST_RUN_INTERNAL(list_remove_not_found);
  TEST_RUN_INTERNAL(list_destroy_null);
  TEST_RUN_INTERNAL(list_destroy_empty);
  TEST_RUN_INTERNAL(list_prev_pointers);

  printf("\n" TEST_GREEN "%d passed" TEST_RESET ", " TEST_RED "%d failed"
         TEST_RESET "\n",
         _test_pass, _test_fail);
  return _test_fail ? 1 : 0;
}
