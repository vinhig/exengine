#include "test.h"
#include <exengine/math/octree.h>
#include <exengine/math/mathlib.h>
#include <stdlib.h>

TEST(rect_new) {
  vec3 min = {-1, -2, -3}, max = {1, 2, 3};
  rect_t r = ex_rect_new(min, max);
  CHECK_VEC3_EQ(r.min, min, 1e-6f);
  CHECK_VEC3_EQ(r.max, max, 1e-6f);
}

TEST(rect_from_triangle) {
  vec3 tri[3] = {{0, 0, 0}, {2, 0, 0}, {0, 3, 0}};
  rect_t r = ex_rect_from_triangle(tri);
  CHECK_VEC3_EQ(r.min, ((vec3){0, 0, 0}), 1e-6f);
  CHECK_VEC3_EQ(r.max, ((vec3){2, 3, 0}), 1e-6f);
}

TEST(aabb_aabb_overlapping) {
  rect_t a = {.min = {0, 0, 0}, .max = {2, 2, 2}};
  rect_t b = {.min = {1, 1, 1}, .max = {3, 3, 3}};
  CHECK_EQ(ex_aabb_aabb(a, b), 1);
}

TEST(aabb_aabb_touching_edge) {
  rect_t a = {.min = {0, 0, 0}, .max = {2, 2, 2}};
  rect_t b = {.min = {2, 0, 0}, .max = {4, 2, 2}};
  CHECK_EQ(ex_aabb_aabb(a, b), 1);
}

TEST(aabb_aabb_disjoint) {
  rect_t a = {.min = {0, 0, 0}, .max = {1, 1, 1}};
  rect_t b = {.min = {10, 10, 10}, .max = {20, 20, 20}};
  CHECK_EQ(ex_aabb_aabb(a, b), 0);
}

TEST(aabb_aabb_disjoint_axis) {
  rect_t a = {.min = {0, 0, 0}, .max = {1, 1, 1}};
  rect_t b = {.min = {2, 0, 0}, .max = {3, 1, 1}};
  CHECK_EQ(ex_aabb_aabb(a, b), 0);
}

TEST(aabb_aabb_inside_other) {
  rect_t a = {.min = {0, 0, 0}, .max = {10, 10, 10}};
  rect_t b = {.min = {2, 2, 2}, .max = {4, 4, 4}};
  CHECK_EQ(ex_aabb_aabb(a, b), 1);
}

TEST(aabb_inside) {
  rect_t outer = {.min = {0, 0, 0}, .max = {10, 10, 10}};
  rect_t inner = {.min = {2, 2, 2}, .max = {8, 8, 8}};
  CHECK_EQ(ex_aabb_inside(outer, inner), 1);
}

TEST(aabb_inside_partial) {
  rect_t outer = {.min = {0, 0, 0}, .max = {5, 5, 5}};
  rect_t inner = {.min = {3, 3, 3}, .max = {10, 10, 10}};
  CHECK_EQ(ex_aabb_inside(outer, inner), 0);
}

TEST(aabb_inside_disjoint) {
  rect_t outer = {.min = {0, 0, 0}, .max = {1, 1, 1}};
  rect_t inner = {.min = {10, 10, 10}, .max = {20, 20, 20}};
  CHECK_EQ(ex_aabb_inside(outer, inner), 0);
}

TEST(rect_intersect_sphere_inside) {
  rect_t r = {.min = {-5, -5, -5}, .max = {5, 5, 5}};
  vec3 pos = {0, 0, 0};
  CHECK_EQ(ex_rect_intersect_sphere(r, pos, 1.0f), 1);
}

TEST(rect_intersect_sphere_partial) {
  rect_t r = {.min = {0, 0, 0}, .max = {10, 10, 10}};
  vec3 pos = {12, 5, 5};
  CHECK_EQ(ex_rect_intersect_sphere(r, pos, 3.0f), 1);
}

TEST(rect_intersect_sphere_far) {
  rect_t r = {.min = {0, 0, 0}, .max = {1, 1, 1}};
  vec3 pos = {100, 100, 100};
  CHECK_EQ(ex_rect_intersect_sphere(r, pos, 1.0f), 0);
}

TEST(rect_intersect_sphere_touching) {
  rect_t r = {.min = {0, 0, 0}, .max = {10, 10, 10}};
  vec3 pos = {5, 5, 11};
  /* sphere of radius 1 centered at z=11 just touches z=10 face */
  CHECK_EQ(ex_rect_intersect_sphere(r, pos, 1.0f), 1);
}

TEST(rect_intersect_sphere_point_inside) {
  rect_t r = {.min = {-1, -1, -1}, .max = {1, 1, 1}};
  vec3 pos = {0.5f, 0.5f, 0.5f};
  /* zero radius point inside AABB should be intersecting */
  CHECK_EQ(ex_rect_intersect_sphere(r, pos, 0.0f), 1);
}

TEST(ex_squared) {
  CHECK_FLOAT_EQ(ex_squared(3.0f), 9.0f, 1e-6f);
  CHECK_FLOAT_EQ(ex_squared(-4.0f), 16.0f, 1e-6f);
  CHECK_FLOAT_EQ(ex_squared(0.0f), 0.0f, 1e-6f);
}

int main(void) {
  TEST_RUN_INTERNAL(rect_new);
  TEST_RUN_INTERNAL(rect_from_triangle);
  TEST_RUN_INTERNAL(aabb_aabb_overlapping);
  TEST_RUN_INTERNAL(aabb_aabb_touching_edge);
  TEST_RUN_INTERNAL(aabb_aabb_disjoint);
  TEST_RUN_INTERNAL(aabb_aabb_disjoint_axis);
  TEST_RUN_INTERNAL(aabb_aabb_inside_other);
  TEST_RUN_INTERNAL(aabb_inside);
  TEST_RUN_INTERNAL(aabb_inside_partial);
  TEST_RUN_INTERNAL(aabb_inside_disjoint);
  TEST_RUN_INTERNAL(rect_intersect_sphere_inside);
  TEST_RUN_INTERNAL(rect_intersect_sphere_partial);
  TEST_RUN_INTERNAL(rect_intersect_sphere_far);
  TEST_RUN_INTERNAL(rect_intersect_sphere_touching);
  TEST_RUN_INTERNAL(rect_intersect_sphere_point_inside);
  TEST_RUN_INTERNAL(ex_squared);

  printf("\n" TEST_GREEN "%d passed" TEST_RESET ", " TEST_RED "%d failed"
         TEST_RESET "\n",
         _test_pass, _test_fail);
  return _test_fail ? 1 : 0;
}
