#include "test.h"
#include <exengine/math/collision.h>
#include <float.h>
#include <math.h>

/* --- Plane tests --- */

TEST(plane_new) {
  vec3 origin = {0, 0, 0}, normal = {0, 1, 0};
  ex_plane_t p = ex_plane_new(origin, normal);
  CHECK_VEC3_EQ(p.origin, origin, 1e-6f);
  CHECK_VEC3_EQ(p.normal, normal, 1e-6f);
  CHECK_FLOAT_EQ(p.equation[3], 0.0f, 1e-6f);
}

TEST(plane_new_offset) {
  vec3 origin = {0, 5, 0}, normal = {0, 1, 0};
  ex_plane_t p = ex_plane_new(origin, normal);
  CHECK_FLOAT_EQ(p.equation[3], -5.0f, 1e-6f);
}

TEST(triangle_to_plane) {
  vec3 a = {0, 0, 0}, b = {1, 0, 0}, c = {0, 0, 1};
  ex_plane_t p = ex_triangle_to_plane(a, b, c);
  CHECK_VEC3_EQ(p.normal, ((vec3){0, -1, 0}), 1e-6f);
}

TEST(triangle_to_plane_degenerate) {
  vec3 a = {0, 0, 0}, b = {1, 0, 0}, c = {2, 0, 0};
  ex_plane_t p = ex_triangle_to_plane(a, b, c);
  CHECK_FLOAT_EQ(vec3_len(p.normal), 0.0f, 1e-6f);
}

TEST(signed_distance_to_plane) {
  vec3 origin = {0, 0, 0}, normal = {0, 1, 0};
  ex_plane_t p = ex_plane_new(origin, normal);
  vec3 above = {0, 5, 0};
  CHECK_FLOAT_EQ(ex_signed_distance_to_plane(above, &p), 5.0f, 1e-6f);
  vec3 below = {0, -5, 0};
  CHECK_FLOAT_EQ(ex_signed_distance_to_plane(below, &p), -5.0f, 1e-6f);
  CHECK_FLOAT_EQ(ex_signed_distance_to_plane(origin, &p), 0.0f, 1e-6f);
}

TEST(is_front_facing) {
  vec3 origin = {0, 0, 0}, normal = {0, 1, 0};
  ex_plane_t p = ex_plane_new(origin, normal);
  vec3 toward = {0, 1, 0};
  CHECK_EQ(ex_is_front_facing(&p, toward), 0);
  vec3 away = {0, -1, 0};
  CHECK_EQ(ex_is_front_facing(&p, away), 1);
  vec3 perpendicular = {1, 0, 0};
  CHECK_EQ(ex_is_front_facing(&p, perpendicular), 1);
}

/* --- Point in triangle --- */

TEST(point_in_triangle_centroid) {
  vec3 a = {0, 0, 0}, b = {2, 0, 0}, c = {0, 2, 0};
  vec3 centroid = {0.5f, 0.5f, 0};
  CHECK_EQ(ex_check_point_in_triangle(centroid, a, b, c), 1);
}

TEST(point_in_triangle_on_edge) {
  vec3 a = {0, 0, 0}, b = {2, 0, 0}, c = {0, 2, 0};
  vec3 edge_mid = {1, 0, 0};
  CHECK_EQ(ex_check_point_in_triangle(edge_mid, a, b, c), 1);
}

TEST(point_in_triangle_on_vertex) {
  vec3 a = {0, 0, 0}, b = {2, 0, 0}, c = {0, 2, 0};
  CHECK_EQ(ex_check_point_in_triangle(a, a, b, c), 1);
  CHECK_EQ(ex_check_point_in_triangle(b, a, b, c), 1);
  CHECK_EQ(ex_check_point_in_triangle(c, a, b, c), 1);
}

TEST(point_in_triangle_outside) {
  vec3 a = {0, 0, 0}, b = {2, 0, 0}, c = {0, 2, 0};
  vec3 outside = {2, 2, 0};
  CHECK_EQ(ex_check_point_in_triangle(outside, a, b, c), 0);
}

/* --- Quadratic solver --- */

TEST(get_lowest_root_two_positive) {
  float root;
  CHECK_EQ(ex_get_lowest_root(1.0f, -5.0f, 6.0f, 100.0f, &root), 1);
  CHECK_FLOAT_EQ(root, 2.0f, 1e-6f);
}

TEST(get_lowest_root_one_positive) {
  float root;
  CHECK_EQ(ex_get_lowest_root(1.0f, -2.0f, -3.0f, 100.0f, &root), 1);
  CHECK_FLOAT_EQ(root, 3.0f, 1e-6f);
}

TEST(get_lowest_root_no_solution) {
  float root;
  CHECK_EQ(ex_get_lowest_root(1.0f, 0.0f, 1.0f, 100.0f, &root), 0);
}

TEST(get_lowest_root_both_negative) {
  float root;
  CHECK_EQ(ex_get_lowest_root(1.0f, 5.0f, 6.0f, 100.0f, &root), 0);
}

TEST(get_lowest_root_max_bound) {
  float root;
  CHECK_EQ(ex_get_lowest_root(1.0f, -5.0f, 6.0f, 2.5f, &root), 1);
  CHECK_FLOAT_EQ(root, 2.0f, 1e-6f);
}

TEST(get_lowest_root_both_above_max) {
  float root;
  CHECK_EQ(ex_get_lowest_root(1.0f, -5.0f, 6.0f, 1.0f, &root), 0);
}

/* --- Ray-triangle intersection (Möller–Trumbore) --- */

TEST(ray_in_tri_hit_center) {
  vec3 from = {0, 0, 5}, to = {0, 0, -1};
  vec3 v0 = {-1, -1, 0}, v1 = {1, -1, 0}, v2 = {0, 1, 0};
  vec3 intersect;
  CHECK_EQ(ray_in_tri(from, to, v0, v1, v2, intersect), 1);
  CHECK_FLOAT_EQ(intersect[2], 0.0f, 1e-6f);
}

TEST(ray_in_tri_miss) {
  vec3 from = {0, 0, 5}, to = {1, 0, 0};
  vec3 v0 = {-1, -1, 0}, v1 = {1, -1, 0}, v2 = {0, 1, 0};
  vec3 intersect;
  CHECK_EQ(ray_in_tri(from, to, v0, v1, v2, intersect), 0);
}

TEST(ray_in_tri_parallel) {
  vec3 from = {0, 0, 5}, to = {1, 0, 0};
  vec3 v0 = {0, 0, 0}, v1 = {1, 0, 0}, v2 = {0, 1, 0};
  vec3 intersect;
  CHECK_EQ(ray_in_tri(from, to, v0, v1, v2, intersect), 0);
}

/* --- Ellipsoid-triangle collision --- */

TEST(collision_check_triangle_no_collision) {
  ex_coll_packet_t packet = {};
  vec3 e_radius = {1, 1, 1};
  memcpy(packet.e_radius, e_radius, sizeof(vec3));
  vec3 e_base = {10, 0, 0};
  memcpy(packet.e_base_point, e_base, sizeof(vec3));
  vec3 e_vel = {1, 0, 0};
  memcpy(packet.e_velocity, e_vel, sizeof(vec3));
  vec3 e_norm = {1, 0, 0};
  memcpy(packet.e_norm_velocity, e_norm, sizeof(vec3));
  packet.found_collision = 0;
  packet.nearest_distance = FLT_MAX;
  packet.t = 0.0;

  vec3 p1 = {0, -10, -10}, p2 = {0, 10, -10}, p3 = {0, 0, 10};
  ex_collision_check_triangle(&packet, p1, p2, p3);
  CHECK_EQ(packet.found_collision, 0);
}

TEST(collision_check_triangle_direct_hit) {
  ex_coll_packet_t packet = {};
  vec3 e_radius = {1, 1, 1};
  memcpy(packet.e_radius, e_radius, sizeof(vec3));
  vec3 e_base = {0, 0, 5};
  memcpy(packet.e_base_point, e_base, sizeof(vec3));
  vec3 e_vel = {0, 0, -5};
  memcpy(packet.e_velocity, e_vel, sizeof(vec3));
  vec3 e_norm = {0, 0, -1};
  memcpy(packet.e_norm_velocity, e_norm, sizeof(vec3));
  packet.found_collision = 0;
  packet.nearest_distance = FLT_MAX;
  packet.t = FLT_MAX;

  vec3 p1 = {-10, -10, 0}, p2 = {10, -10, 0}, p3 = {0, 10, 0};
  ex_collision_check_triangle(&packet, p1, p2, p3);
  CHECK_EQ(packet.found_collision, 1);
  CHECK(packet.nearest_distance >= 0.0f);
  CHECK(packet.t > 0.0);
}

TEST(collision_check_triangle_backface) {
  ex_coll_packet_t packet = {};
  vec3 e_radius = {1, 1, 1};
  memcpy(packet.e_radius, e_radius, sizeof(vec3));
  vec3 e_base = {0, 0, -5};
  memcpy(packet.e_base_point, e_base, sizeof(vec3));
  vec3 e_vel = {0, 0, 5};
  memcpy(packet.e_velocity, e_vel, sizeof(vec3));
  vec3 e_norm = {0, 0, 1};
  memcpy(packet.e_norm_velocity, e_norm, sizeof(vec3));
  packet.found_collision = 0;
  packet.nearest_distance = FLT_MAX;
  packet.t = FLT_MAX;

  vec3 p1 = {-10, -10, 0}, p2 = {10, -10, 0}, p3 = {0, 10, 0};
  ex_collision_check_triangle(&packet, p1, p2, p3);
  CHECK_EQ(packet.found_collision, 0);
}

int main(void) {
  TEST_RUN_INTERNAL(plane_new);
  TEST_RUN_INTERNAL(plane_new_offset);
  TEST_RUN_INTERNAL(triangle_to_plane);
  TEST_RUN_INTERNAL(triangle_to_plane_degenerate);
  TEST_RUN_INTERNAL(signed_distance_to_plane);
  TEST_RUN_INTERNAL(is_front_facing);
  TEST_RUN_INTERNAL(point_in_triangle_centroid);
  TEST_RUN_INTERNAL(point_in_triangle_on_edge);
  TEST_RUN_INTERNAL(point_in_triangle_on_vertex);
  TEST_RUN_INTERNAL(point_in_triangle_outside);
  TEST_RUN_INTERNAL(get_lowest_root_two_positive);
  TEST_RUN_INTERNAL(get_lowest_root_one_positive);
  TEST_RUN_INTERNAL(get_lowest_root_no_solution);
  TEST_RUN_INTERNAL(get_lowest_root_both_negative);
  TEST_RUN_INTERNAL(get_lowest_root_max_bound);
  TEST_RUN_INTERNAL(get_lowest_root_both_above_max);
  TEST_RUN_INTERNAL(ray_in_tri_hit_center);
  TEST_RUN_INTERNAL(ray_in_tri_miss);
  TEST_RUN_INTERNAL(ray_in_tri_parallel);
  TEST_RUN_INTERNAL(collision_check_triangle_no_collision);
  TEST_RUN_INTERNAL(collision_check_triangle_direct_hit);
  TEST_RUN_INTERNAL(collision_check_triangle_backface);

  printf("\n" TEST_GREEN "%d passed" TEST_RESET ", " TEST_RED "%d failed"
         TEST_RESET "\n",
         _test_pass, _test_fail);
  return _test_fail ? 1 : 0;
}
