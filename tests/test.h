/* Minimal single-header test framework for exengine unit tests.
 * Usage:
 *   #include "test.h"
 *   TEST(my_test) { ... }
 *   int main(void) { return TEST_RUN(); }
 *
 * Compile as C or C++.
 */

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TEST_NO_COLOR
#define TEST_RED "\033[0;31m"
#define TEST_GREEN "\033[0;32m"
#define TEST_YELLOW "\033[0;33m"
#define TEST_RESET "\033[0m"
#else
#define TEST_RED ""
#define TEST_GREEN ""
#define TEST_YELLOW ""
#define TEST_RESET ""
#endif

static int _test_pass = 0;
static int _test_fail = 0;
static const char *_test_name = NULL;

#define TEST(name)                                           \
  static void test_##name(void);                             \
  static void test_##name(void)

#define TEST_RUN_INTERNAL(name)                              \
  do {                                                       \
    _test_name = #name;                                      \
    test_##name();                                           \
  } while (0)

#define TEST_RUN()                                           \
  ({                                                         \
    int _run_pass = 0, _run_fail = 0;                        \
    void *_run_tests[] = { NULL };                           \
    (void)_run_tests;                                        \
    TEST_REGISTRY                                     \
    printf("\n" TEST_GREEN "%d passed" TEST_RESET            \
           ", " TEST_RED "%d failed" TEST_RESET "\n",        \
           _test_pass, _test_fail);                          \
    _run_pass = _test_pass; _run_fail = _test_fail;          \
    _test_fail ? 1 : 0;                                      \
  })

#define CHECK_MSG(cond, ...)                                 \
  do {                                                       \
    if (!(cond)) {                                           \
      fprintf(stderr, TEST_RED "FAIL" TEST_RESET             \
              " %s:%d: %s: ",                                \
              __FILE__, __LINE__, _test_name);               \
      fprintf(stderr, __VA_ARGS__);                          \
      fprintf(stderr, "\n");                                 \
      _test_fail++;                                          \
      return;                                                \
    }                                                        \
    _test_pass++;                                            \
  } while (0)

#define CHECK(cond) CHECK_MSG(cond, "expected: %s", #cond)

#define CHECK_EQ(a, b)                                       \
  do {                                                       \
    typeof(a) _a = (a);                                      \
    typeof(b) _b = (b);                                      \
    if (!(_a == _b)) {                                       \
      fprintf(stderr, TEST_RED "FAIL" TEST_RESET             \
              " %s:%d: %s: %s == %s\n",                      \
              __FILE__, __LINE__, _test_name, #a, #b);       \
      fprintf(stderr, "  left: %g  right: %g\n",             \
              (double)_a, (double)_b);                       \
      _test_fail++;                                          \
      return;                                                \
    }                                                        \
    _test_pass++;                                            \
  } while (0)

#define CHECK_FLOAT_EQ(a, b, eps)                            \
  do {                                                       \
    float _a = (a);                                          \
    float _b = (b);                                          \
    if (fabsf(_a - _b) > (eps)) {                            \
      fprintf(stderr, TEST_RED "FAIL" TEST_RESET             \
              " %s:%d: %s: %s ~= %s\n",                      \
              __FILE__, __LINE__, _test_name, #a, #b);       \
      fprintf(stderr, "  left: %.9g  right: %.9g  eps: %g\n",\
              _a, _b, (float)(eps));                         \
      _test_fail++;                                          \
      return;                                                \
    }                                                        \
    _test_pass++;                                            \
  } while (0)

#define CHECK_VEC3_EQ(a, b, eps)                             \
  do {                                                       \
    CHECK_FLOAT_EQ((a)[0], (b)[0], eps);                     \
    CHECK_FLOAT_EQ((a)[1], (b)[1], eps);                     \
    CHECK_FLOAT_EQ((a)[2], (b)[2], eps);                     \
  } while (0)

#define CHECK_VEC4_EQ(a, b, eps)                             \
  do {                                                       \
    CHECK_FLOAT_EQ((a)[0], (b)[0], eps);                     \
    CHECK_FLOAT_EQ((a)[1], (b)[1], eps);                     \
    CHECK_FLOAT_EQ((a)[2], (b)[2], eps);                     \
    CHECK_FLOAT_EQ((a)[3], (b)[3], eps);                     \
  } while (0)

#define CHECK_MAT4_EQ(a, b, eps)                             \
  do {                                                       \
    for (int _mi = 0; _mi < 4; _mi++)                        \
      CHECK_VEC4_EQ((a)[_mi], (b)[_mi], eps);                \
  } while (0)

#define CHECK_STR_EQ(a, b)                                   \
  do {                                                       \
    if (strcmp((a), (b)) != 0) {                             \
      fprintf(stderr, TEST_RED "FAIL" TEST_RESET             \
              " %s:%d: %s: %s == %s\n",                      \
              __FILE__, __LINE__, _test_name, #a, #b);       \
      fprintf(stderr, "  left: \"%s\"  right: \"%s\"\n",     \
              (a), (b));                                     \
      _test_fail++;                                          \
      return;                                                \
    }                                                        \
    _test_pass++;                                            \
  } while (0)

#define CHECK_NULL(p)  CHECK_MSG((p) == NULL, "expected NULL: %s", #p)
#define CHECK_NOT_NULL(p) CHECK_MSG((p) != NULL, "expected non-NULL: %s", #p)

/* End of test.h */
