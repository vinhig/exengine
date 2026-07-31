#include "test.h"
#include <exengine/util/ini.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ex_ini_t is ~32MB; use a static buffer to avoid heap alloc / ASAN leaks */
static ex_ini_t _g_ini_store;

static ex_ini_t *make_ini(void) {
  memset(&_g_ini_store, 0, sizeof(_g_ini_store));
  return &_g_ini_store;
}

static void free_ini(ex_ini_t *ini) {
  (void)ini;
}

TEST(parse_empty) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, ""));
  CHECK_EQ(ini->length, 0);
  free_ini(ini);
}

TEST(parse_null) {
  ex_ini_t *ini = make_ini();
  CHECK_EQ(ex_ini_parse(ini, NULL), 0);
  CHECK_EQ(ini->length, 0);
  free_ini(ini);
}

TEST(parse_one_section_one_key) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[graphics]\nwidth=1280\n"));
  CHECK_EQ(ini->length, 1);
  CHECK_STR_EQ(ini->sections[0].name, "graphics");
  CHECK_EQ(ini->sections[0].length, 1);
  CHECK_STR_EQ(ini->sections[0].vars[0].key, "width");
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_float);
  CHECK_FLOAT_EQ(ini->sections[0].vars[0].f, 1280.0f, 1e-6f);
  free_ini(ini);
}

TEST(parse_float_value) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[test]\nval=42.5\n"));
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_float);
  CHECK_FLOAT_EQ(ini->sections[0].vars[0].f, 42.5f, 1e-6f);
  free_ini(ini);
}

TEST(parse_negative_float) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[test]\nval=-3.14\n"));
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_float);
  CHECK_FLOAT_EQ(ini->sections[0].vars[0].f, -3.14f, 1e-6f);
  free_ini(ini);
}

TEST(parse_string_value) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[test]\nname=hello\n"));
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_string);
  CHECK_STR_EQ(ini->sections[0].vars[0].s, "hello");
  free_ini(ini);
}

TEST(parse_multiple_sections) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[s1]\nkey=value\n[s2]\nother=99\n"));
  CHECK_EQ(ini->length, 2);
  CHECK_STR_EQ(ini->sections[0].name, "s1");
  CHECK_STR_EQ(ini->sections[1].name, "s2");
  free_ini(ini);
}

TEST(parse_multiple_keys_in_section) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[cfg]\na=1\nb=hello\nc=3.14\n"));
  CHECK_EQ(ini->sections[0].length, 3);
  CHECK_STR_EQ(ini->sections[0].vars[0].key, "a");
  CHECK_STR_EQ(ini->sections[0].vars[1].key, "b");
  CHECK_STR_EQ(ini->sections[0].vars[2].key, "c");
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_float);
  CHECK_EQ(ini->sections[0].vars[1].type, ex_ini_type_string);
  CHECK_EQ(ini->sections[0].vars[2].type, ex_ini_type_float);
  free_ini(ini);
}

TEST(parse_duplicate_key_overwrites) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[cfg]\nkey=first\nkey=second\n"));
  CHECK_EQ(ini->sections[0].length, 1);
  CHECK_STR_EQ(ini->sections[0].vars[0].s, "second");
  free_ini(ini);
}

TEST(parse_key_without_section_ignored) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "key=value\n"));
  CHECK_EQ(ini->length, 0);
  free_ini(ini);
}

TEST(parse_whitespace_stripping) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "  [  section  ]  \n  key  =  value  \n"));
  CHECK_EQ(ini->length, 1);
  CHECK_STR_EQ(ini->sections[0].name, "section");
  CHECK_STR_EQ(ini->sections[0].vars[0].key, "key");
  free_ini(ini);
}

TEST(parse_equals_in_value) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[test]\nkey=foo=bar\n"));
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_string);
  CHECK_STR_EQ(ini->sections[0].vars[0].s, "foo=bar");
  free_ini(ini);
}

TEST(parse_empty_value) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[test]\nkey=\n"));
  CHECK_EQ(ini->sections[0].vars[0].type, ex_ini_type_float);
  CHECK_FLOAT_EQ(ini->sections[0].vars[0].f, 0.0f, 1e-6f);
  free_ini(ini);
}

TEST(parse_unclosed_section) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[incomplete\nkey=val\n"));
  CHECK_EQ(ini->length, 0);
  free_ini(ini);
}

TEST(parse_empty_section_name) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[]\nkey=val\n"));
  CHECK_EQ(ini->length, 1);
  CHECK_STR_EQ(ini->sections[0].name, "");
  free_ini(ini);
}

TEST(ini_exists) {
  ex_ini_t *ini = make_ini();
  ex_ini_parse(ini, "[cfg]\nkey=value\n");
  CHECK(ex_ini_exists(ini, "cfg", "key"));
  CHECK(!ex_ini_exists(ini, "cfg", "nonexistent"));
  CHECK(!ex_ini_exists(ini, "nonexistent", "key"));
  free_ini(ini);
}

TEST(ini_get_string) {
  ex_ini_t *ini = make_ini();
  ex_ini_parse(ini, "[cfg]\ntext=hello\n");
  CHECK_STR_EQ(ex_ini_get_string(ini, "cfg", "text"), "hello");
  free_ini(ini);
}

TEST(ini_get_float) {
  ex_ini_t *ini = make_ini();
  ex_ini_parse(ini, "[cfg]\nnum=42.5\n");
  CHECK_FLOAT_EQ(ex_ini_get_float(ini, "cfg", "num"), 42.5f, 1e-6f);
  free_ini(ini);
}

TEST(ini_get_float_missing_returns_zero) {
  ex_ini_t *ini = make_ini();
  CHECK_FLOAT_EQ(ex_ini_get_float(ini, "cfg", "missing"), 0.0f, 1e-6f);
  free_ini(ini);
}

TEST(ini_get_bool) {
  ex_ini_t *ini = make_ini();
  ex_ini_parse(ini, "[cfg]\nflag=true\n");
  CHECK(ex_ini_get_bool(ini, "cfg", "flag"));
  ex_ini_parse(ini, "[cfg]\nflag=false\n");
  CHECK(!ex_ini_get_bool(ini, "cfg", "flag"));
  free_ini(ini);
}

TEST(ini_get_bool_case_sensitive) {
  ex_ini_t *ini = make_ini();
  ex_ini_parse(ini, "[cfg]\nflag=TRUE\n");
  CHECK(!ex_ini_get_bool(ini, "cfg", "flag"));
  free_ini(ini);
}

TEST(ini_set_string) {
  ex_ini_t *ini = make_ini();
  ex_ini_set_string(ini, "cfg", "key", "newvalue");
  CHECK_STR_EQ(ex_ini_get_string(ini, "cfg", "key"), "newvalue");
  free_ini(ini);
}

TEST(ini_set_float) {
  ex_ini_t *ini = make_ini();
  ex_ini_set_float(ini, "cfg", "key", 3.14f);
  CHECK_FLOAT_EQ(ex_ini_get_float(ini, "cfg", "key"), 3.14f, 1e-6f);
  free_ini(ini);
}

TEST(ini_set_bool_true) {
  ex_ini_t *ini = make_ini();
  ex_ini_set_bool(ini, "cfg", "key", true);
  CHECK(ex_ini_get_bool(ini, "cfg", "key"));
  free_ini(ini);
}

TEST(ini_set_bool_false) {
  ex_ini_t *ini = make_ini();
  ex_ini_set_bool(ini, "cfg", "key", false);
  CHECK(!ex_ini_get_bool(ini, "cfg", "key"));
  free_ini(ini);
}

TEST(parse_idempotent) {
  ex_ini_t *ini = make_ini();
  CHECK(ex_ini_parse(ini, "[cfg]\na=1\nb=two\n"));
  CHECK(ex_ini_parse(ini, "[cfg]\nc=3.0\n"));
  CHECK_EQ(ini->length, 1);
  CHECK_EQ(ini->sections[0].length, 1);
  CHECK_STR_EQ(ini->sections[0].vars[0].key, "c");
  free_ini(ini);
}

TEST(ini_get_var_creates_new) {
  ex_ini_t *ini = make_ini();
  ex_ini_var_t *v = ex_ini_get_var(ini, "sec", "key");
  CHECK_NOT_NULL(v);
  CHECK_EQ(ini->length, 1);
  CHECK_STR_EQ(ini->sections[0].name, "sec");
  CHECK_EQ(ini->sections[0].length, 1);
  CHECK_STR_EQ(v->key, "key");
  free_ini(ini);
}

TEST(ini_get_string_empty_returns_empty) {
  ex_ini_t *ini = make_ini();
  CHECK_STR_EQ(ex_ini_get_string(ini, "nosection", "nokey"), "");
  free_ini(ini);
}

int main(void) {
  TEST_RUN_INTERNAL(parse_empty);
  TEST_RUN_INTERNAL(parse_null);
  TEST_RUN_INTERNAL(parse_one_section_one_key);
  TEST_RUN_INTERNAL(parse_float_value);
  TEST_RUN_INTERNAL(parse_negative_float);
  TEST_RUN_INTERNAL(parse_string_value);
  TEST_RUN_INTERNAL(parse_multiple_sections);
  TEST_RUN_INTERNAL(parse_multiple_keys_in_section);
  TEST_RUN_INTERNAL(parse_duplicate_key_overwrites);
  TEST_RUN_INTERNAL(parse_key_without_section_ignored);
  TEST_RUN_INTERNAL(parse_whitespace_stripping);
  TEST_RUN_INTERNAL(parse_equals_in_value);
  TEST_RUN_INTERNAL(parse_empty_value);
  TEST_RUN_INTERNAL(parse_unclosed_section);
  TEST_RUN_INTERNAL(parse_empty_section_name);
  TEST_RUN_INTERNAL(ini_exists);
  TEST_RUN_INTERNAL(ini_get_string);
  TEST_RUN_INTERNAL(ini_get_float);
  TEST_RUN_INTERNAL(ini_get_float_missing_returns_zero);
  TEST_RUN_INTERNAL(ini_get_bool);
  TEST_RUN_INTERNAL(ini_get_bool_case_sensitive);
  TEST_RUN_INTERNAL(ini_set_string);
  TEST_RUN_INTERNAL(ini_set_float);
  TEST_RUN_INTERNAL(ini_set_bool_true);
  TEST_RUN_INTERNAL(ini_set_bool_false);
  TEST_RUN_INTERNAL(parse_idempotent);
  TEST_RUN_INTERNAL(ini_get_var_creates_new);
  TEST_RUN_INTERNAL(ini_get_string_empty_returns_empty);

  printf("\n" TEST_GREEN "%d passed" TEST_RESET ", " TEST_RED "%d failed"
         TEST_RESET "\n",
         _test_pass, _test_fail);
  return _test_fail ? 1 : 0;
}
