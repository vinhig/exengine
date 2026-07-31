#include "test.h"
#include <exengine/math/mathlib.h>

/* --- vec2 / vec3 / vec4 basic ops --- */

TEST(vec3_add) {
  vec3 a = {1, 2, 3}, b = {4, 5, 6}, r;
  vec3_add(r, a, b);
  CHECK_VEC3_EQ(r, ((vec3){5, 7, 9}), 1e-6f);
}

TEST(vec3_sub) {
  vec3 a = {4, 5, 6}, b = {1, 2, 3}, r;
  vec3_sub(r, a, b);
  CHECK_VEC3_EQ(r, ((vec3){3, 3, 3}), 1e-6f);
}

TEST(vec3_scale) {
  vec3 v = {1, 2, 3}, r;
  vec3_scale(r, v, 2.0f);
  CHECK_VEC3_EQ(r, ((vec3){2, 4, 6}), 1e-6f);
  vec3_scale(r, v, 0.0f);
  CHECK_VEC3_EQ(r, ((vec3){0, 0, 0}), 1e-6f);
  vec3_scale(r, v, -1.0f);
  CHECK_VEC3_EQ(r, ((vec3){-1, -2, -3}), 1e-6f);
}

TEST(vec3_mul_inner) {
  vec3 a = {1, 0, 0}, b = {0, 1, 0};
  CHECK_FLOAT_EQ(vec3_mul_inner(a, b), 0.0f, 1e-6f);
  CHECK_FLOAT_EQ(vec3_mul_inner(a, a), 1.0f, 1e-6f);
  vec3 c = {2, 3, 4};
  CHECK_FLOAT_EQ(vec3_mul_inner(c, c), 29.0f, 1e-6f);
}

TEST(vec3_len) {
  vec3 v = {3, 0, 0};
  CHECK_FLOAT_EQ(vec3_len(v), 3.0f, 1e-6f);
  vec3 zero = {0, 0, 0};
  CHECK_FLOAT_EQ(vec3_len(zero), 0.0f, 1e-6f);
  vec3 unit = {0.57735f, 0.57735f, 0.57735f};
  CHECK_FLOAT_EQ(vec3_len(unit), 1.0f, 0.001f);
}

TEST(vec3_norm) {
  vec3 v = {3, 0, 0}, r;
  vec3_norm(r, v);
  CHECK_VEC3_EQ(r, ((vec3){1, 0, 0}), 1e-6f);
  vec3 arbitrary = {2, 3, 4};
  vec3_norm(r, arbitrary);
  CHECK_FLOAT_EQ(vec3_len(r), 1.0f, 1e-6f);
}

TEST(vec3_norm_zero_vector) {
  vec3 zero = {0, 0, 0}, r;
  vec3_norm(r, zero);
  CHECK(r[0] != r[0] || r[1] != r[1] || r[2] != r[2]);
}

TEST(vec3_min_max) {
  vec3 a = {1, 5, 3}, b = {4, 2, 6}, r;
  vec3_min(r, a, b);
  CHECK_VEC3_EQ(r, ((vec3){1, 2, 3}), 1e-6f);
  vec3_max(r, a, b);
  CHECK_VEC3_EQ(r, ((vec3){4, 5, 6}), 1e-6f);
}

TEST(vec3_mul_cross) {
  vec3 x = {1, 0, 0}, y = {0, 1, 0}, r;
  vec3_mul_cross(r, x, y);
  CHECK_VEC3_EQ(r, ((vec3){0, 0, 1}), 1e-6f);
  vec3_mul_cross(r, y, x);
  CHECK_VEC3_EQ(r, ((vec3){0, 0, -1}), 1e-6f);
  vec3_mul_cross(r, x, x);
  CHECK_VEC3_EQ(r, ((vec3){0, 0, 0}), 1e-6f);
}

TEST(vec3_reflect) {
  vec3 v = {1, -1, 0}, n = {0, 1, 0}, r;
  vec3_reflect(r, v, n);
  CHECK_VEC3_EQ(r, ((vec3){1, 1, 0}), 1e-6f);
}

TEST(vec3_lerp) {
  vec3 a = {0, 0, 0}, b = {10, 10, 10}, r;
  vec3_lerp(r, a, b, 0.0f);
  CHECK_VEC3_EQ(r, a, 1e-6f);
  vec3_lerp(r, a, b, 1.0f);
  CHECK_VEC3_EQ(r, b, 1e-6f);
  vec3_lerp(r, a, b, 0.5f);
  CHECK_VEC3_EQ(r, ((vec3){5, 5, 5}), 1e-6f);
}

TEST(vec3_div) {
  vec3 a = {6, 8, 10}, b = {2, 4, 5}, r;
  vec3_div(r, a, b);
  CHECK_VEC3_EQ(r, ((vec3){3, 2, 2}), 1e-6f);
}

TEST(vec3_trim_to_shorter) {
  vec3 v = {3, 0, 0}, r;
  vec3_trim(r, v, 1.0f);
  /* Trimming to length 1 should produce {1, 0, 0} */
  CHECK_VEC3_EQ(r, ((vec3){1, 0, 0}), 1e-6f);
}

TEST(vec3_clamp) {
  vec3 v = {0, 5, 10}, r;
  vec3_clamp(r, v, 2.0f, 8.0f);
  CHECK_VEC3_EQ(r, ((vec3){2, 5, 8}), 1e-6f);
}

TEST(vec2_len) {
  vec2 v = {3, 4};
  CHECK_FLOAT_EQ(vec2_len(v), 5.0f, 1e-6f);
}

TEST(vec2_cross) {
  vec2 a = {1, 0}, b = {0, 1};
  CHECK_FLOAT_EQ(cross(a, b), 1.0, 1e-9);
  CHECK_FLOAT_EQ(cross(b, a), -1.0, 1e-9);
}

TEST(vec4_mul_cross) {
  vec4 a = {1, 0, 0, 0}, b = {0, 1, 0, 0}, r;
  vec4_mul_cross(r, a, b);
  CHECK_VEC4_EQ(r, ((vec4){0, 0, 1, 1}), 1e-6f);
}

/* --- mat4x4 --- */

TEST(mat4x4_identity) {
  mat4x4 M;
  mat4x4_identity(M);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      CHECK_FLOAT_EQ(M[i][j], i == j ? 1.0f : 0.0f, 1e-6f);
}

TEST(mat4x4_dup) {
  mat4x4 A, B;
  mat4x4_identity(A);
  mat4x4_dup(B, A);
  CHECK_MAT4_EQ(A, B, 1e-6f);
}

TEST(mat4x4_transpose) {
  mat4x4 M = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  mat4x4 T;
  mat4x4_transpose(T, M);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      CHECK_FLOAT_EQ(T[i][j], M[j][i], 1e-6f);
}

TEST(mat4x4_mul_identity) {
  mat4x4 I, M, R;
  mat4x4_identity(I);
  mat4x4 Mtest = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  mat4x4_dup(M, Mtest);
  mat4x4_mul(R, M, I);
  CHECK_MAT4_EQ(R, M, 1e-6f);
  mat4x4_mul(R, I, M);
  CHECK_MAT4_EQ(R, M, 1e-6f);
}

TEST(mat4x4_mul_known) {
  mat4x4 A = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {3, 0, 0, 1}};
  mat4x4 B = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 4, 0, 1}};
  mat4x4 R;
  mat4x4_mul(R, A, B);
  CHECK_FLOAT_EQ(R[3][1], 4.0f, 1e-6f);
  CHECK_FLOAT_EQ(R[3][0], 3.0f, 1e-6f);
}

TEST(mat4x4_mul_vec4) {
  mat4x4 I;
  mat4x4_identity(I);
  vec4 v = {1, 2, 3, 1}, r;
  mat4x4_mul_vec4(r, I, v);
  CHECK_VEC4_EQ(r, v, 1e-6f);
}

TEST(mat4x4_translate_mul_vec4) {
  mat4x4 T;
  vec3 t = {5, -3, 2};
  mat4x4_translate(T, t);
  vec4 origin = {0, 0, 0, 1}, r;
  mat4x4_mul_vec4(r, T, origin);
  /* A translation matrix applied to the origin should produce the translation vector */
  CHECK_VEC4_EQ(r, ((vec4){5, -3, 2, 1}), 1e-6f);
}

TEST(mat4x4_translate_in_place) {
  mat4x4 M;
  mat4x4_identity(M);
  mat4x4_translate_in_place(M, 1, 2, 3);
  mat4x4_translate_in_place(M, 1, 2, 3);
  CHECK_FLOAT_EQ(M[3][0], 2.0f, 1e-6f);
  CHECK_FLOAT_EQ(M[3][1], 4.0f, 1e-6f);
  CHECK_FLOAT_EQ(M[3][2], 6.0f, 1e-6f);
}

TEST(mat4x4_scale_xyz) {
  mat4x4 M;
  vec3 s = {2, 3, 4};
  mat4x4_scale_xyz(M, s);
  CHECK_FLOAT_EQ(M[0][0], 2.0f, 1e-6f);
  CHECK_FLOAT_EQ(M[1][1], 3.0f, 1e-6f);
  CHECK_FLOAT_EQ(M[2][2], 4.0f, 1e-6f);
}

TEST(mat4x4_invert_identity) {
  mat4x4 I, inv;
  mat4x4_identity(I);
  mat4x4_invert(inv, I);
  CHECK_MAT4_EQ(inv, I, 1e-6f);
}

TEST(mat4x4_invert_roundtrip) {
  mat4x4 M = {{1, 0, 0, 4}, {0, 1, 0, 5}, {0, 0, 1, 6}, {0, 0, 0, 1}};
  mat4x4 inv, R;
  mat4x4_invert(inv, M);
  mat4x4_mul(R, M, inv);
  mat4x4 I;
  mat4x4_identity(I);
  CHECK_MAT4_EQ(R, I, 1e-3f);
}

TEST(mat4x4_invert_scale) {
  mat4x4 M;
  mat4x4_identity(M);
  M[0][0] = 2.0f; M[1][1] = 4.0f; M[2][2] = 8.0f;
  mat4x4 inv;
  mat4x4_invert(inv, M);
  CHECK_FLOAT_EQ(inv[0][0], 0.5f, 1e-6f);
  CHECK_FLOAT_EQ(inv[1][1], 0.25f, 1e-6f);
  CHECK_FLOAT_EQ(inv[2][2], 0.125f, 1e-6f);
}

TEST(mat4x4_rotate_X_quarter) {
  mat4x4 I, R;
  mat4x4_identity(I);
  mat4x4_rotate_X(R, I, M_PI / 2.0f);
  vec4 v = {0, 1, 0, 0}, r;
  mat4x4_mul_vec4(r, R, v);
  CHECK_VEC4_EQ(r, ((vec4){0, 0, 1, 0}), 1e-6f);
}

TEST(mat4x4_rotate_Y_quarter) {
  mat4x4 I, R;
  mat4x4_identity(I);
  mat4x4_rotate_Y(R, I, M_PI / 2.0f);
  vec4 v = {1, 0, 0, 0}, r;
  mat4x4_mul_vec4(r, R, v);
  /* linmath rotate_Y: x(1,0,0) around Y by +90° maps to z (0,0,1) */
  CHECK_VEC4_EQ(r, ((vec4){0, 0, 1, 0}), 1e-6f);
}

TEST(mat4x4_rotate_Z_quarter) {
  mat4x4 I, R;
  mat4x4_identity(I);
  mat4x4_rotate_Z(R, I, M_PI / 2.0f);
  vec4 v = {1, 0, 0, 0}, r;
  mat4x4_mul_vec4(r, R, v);
  CHECK_VEC4_EQ(r, ((vec4){0, 1, 0, 0}), 1e-6f);
}

TEST(mat4x4_rotate_axis_angle) {
  mat4x4 I, R;
  mat4x4_identity(I);
  mat4x4_rotate(R, I, 0, 1, 0, M_PI / 2.0f);
  vec4 v = {1, 0, 0, 0}, r;
  mat4x4_mul_vec4(r, R, v);
  CHECK_VEC4_EQ(r, ((vec4){0, 0, -1, 0}), 1e-6f);
}

TEST(mat4x4_rotate_zero_axis) {
  mat4x4 I, R;
  mat4x4_identity(I);
  mat4x4_rotate(R, I, 0, 0, 0, M_PI);
  CHECK_MAT4_EQ(R, I, 1e-6f);
}

TEST(mat4x4_frustum) {
  mat4x4 M;
  mat4x4_frustum(M, -1, 1, -1, 1, 1, 100);
  CHECK_FLOAT_EQ(M[2][2], -(100.0f + 1) / (100.0f - 1), 1e-6f);
}

TEST(mat4x4_ortho) {
  mat4x4 M;
  mat4x4_ortho(M, -10, 10, -10, 10, -1, 1);
  CHECK_FLOAT_EQ(M[0][0], 0.1f, 1e-6f);
  CHECK_FLOAT_EQ(M[1][1], 0.1f, 1e-6f);
}

TEST(mat4x4_perspective) {
  mat4x4 M;
  mat4x4_perspective(M, M_PI / 2.0f, 16.0f / 9.0f, 0.1f, 100.0f);
  CHECK_FLOAT_EQ(M[0][0], 9.0f / 16.0f, 1e-6f);
}

TEST(mat4x4_look_at) {
  mat4x4 V;
  vec3 eye = {0, 0, 5}, center = {0, 0, 0}, up = {0, 1, 0};
  mat4x4_look_at(V, eye, center, up);
  vec4 p = {0, 0, 0, 1}, r;
  mat4x4_mul_vec4(r, V, p);
  /* center maps to ~origin in view space; note mat4x4_look_at uses
   * translate_in_place convention for the translation portion */
  CHECK_FLOAT_EQ(r[0], 0.0f, 1e-5f);
  CHECK_FLOAT_EQ(r[1], 0.0f, 1e-5f);
}

TEST(mat4x4_look_at_no_crash_parallel_up) {
  mat4x4 V;
  vec3 eye = {0, 0, 5}, center = {0, 0, 0}, up = {0, 0, 1};
  mat4x4_look_at(V, eye, center, up);
  /* parallel up should produce finite values, not NaN */
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      CHECK(isfinite(V[i][j]));
}

TEST(mat4x4_orthonormalize) {
  mat4x4 M, R;
  mat4x4_identity(M);
  M[0][0] = 1; M[0][1] = 0.5f; M[0][2] = 0;
  M[1][0] = 0.5f; M[1][1] = 1; M[1][2] = 0;
  M[2][0] = 0; M[2][1] = 0; M[2][2] = 1;
  mat4x4_orthonormalize(R, M);
  CHECK_FLOAT_EQ(vec3_len(R[0]), 1.0f, 0.01f);
  CHECK_FLOAT_EQ(vec3_len(R[1]), 1.0f, 0.01f);
  CHECK_FLOAT_EQ(vec3_len(R[2]), 1.0f, 0.01f);
  CHECK_FLOAT_EQ(vec3_mul_inner(R[0], R[1]), 0.0f, 0.01f);
}

/* --- quat --- */

TEST(quat_identity) {
  quat q;
  quat_identity(q);
  CHECK_FLOAT_EQ(q[0], 0.0f, 1e-6f);
  CHECK_FLOAT_EQ(q[1], 0.0f, 1e-6f);
  CHECK_FLOAT_EQ(q[2], 0.0f, 1e-6f);
  CHECK_FLOAT_EQ(q[3], 1.0f, 1e-6f);
}

TEST(quat_identity_rotate_vec3) {
  quat q;
  quat_identity(q);
  vec3 v = {1, 2, 3}, r;
  quat_mul_vec3(r, q, v);
  CHECK_VEC3_EQ(r, v, 1e-6f);
}

TEST(quat_mul_vec3_180) {
  quat q;
  vec3 axis = {0, 1, 0};
  quat_rotate(q, M_PI, axis);
  vec3 v = {1, 0, 0}, r;
  quat_mul_vec3(r, q, v);
  CHECK_VEC3_EQ(r, ((vec3){-1, 0, 0}), 1e-6f);
}

TEST(quat_mul_vec3_90_y) {
  quat q;
  vec3 axis = {0, 1, 0};
  quat_rotate(q, M_PI / 2.0f, axis);
  vec3 v = {1, 0, 0}, r;
  quat_mul_vec3(r, q, v);
  CHECK_VEC3_EQ(r, ((vec3){0, 0, -1}), 1e-6f);
}

TEST(quat_slerp_weight_zero) {
  quat from, to, r;
  quat_identity(from);
  vec3 axis = {0, 1, 0};
  quat_rotate(to, M_PI / 2.0f, axis);
  quat_slerp(r, from, to, 0.0f);
  CHECK_VEC4_EQ(r, from, 1e-4f);
}

TEST(quat_slerp_weight_one) {
  quat from, to, r;
  quat_identity(from);
  vec3 axis = {0, 1, 0};
  quat_rotate(to, M_PI / 2.0f, axis);
  quat_slerp(r, from, to, 1.0f);
  CHECK_VEC4_EQ(r, to, 1e-4f);
}

TEST(quat_slerp_midpoint) {
  quat from, to, r;
  quat_identity(from);
  vec3 axis = {0, 1, 0};
  quat_rotate(to, M_PI / 2.0f, axis);
  quat_slerp(r, from, to, 0.5f);
  quat mid;
  quat_rotate(mid, M_PI / 4.0f, axis);
  CHECK_VEC4_EQ(r, mid, 1e-4f);
}

TEST(quat_slerp_opposite) {
  quat from, to, r;
  quat_identity(from);
  vec3 axis = {0, 1, 0};
  quat_rotate(to, M_PI, axis);
  quat_slerp(r, from, to, 0.0f);
  CHECK_VEC4_EQ(r, from, 1e-3f);
}

TEST(quat_slerp_near_parallel) {
  quat from, to, r;
  quat_identity(from);
  vec3 axis = {0, 0.01f, 1};
  quat_rotate(to, 0.001f, axis);
  quat_slerp(r, from, to, 0.5f);
  CHECK_FLOAT_EQ(vec4_len(r), 1.0f, 1e-3f);
}

TEST(quat_mul) {
  quat a, b, r;
  quat_identity(a);
  vec3 axis = {0, 1, 0};
  quat_rotate(b, M_PI / 2.0f, axis);
  quat_mul(r, a, b);
  CHECK_VEC4_EQ(r, b, 1e-4f);
}

TEST(quat_from_mat4x4_identity_roundtrip) {
  mat4x4 I;
  mat4x4_identity(I);
  quat q;
  quat_from_mat4x4(q, I);
  /* Identity matrix should produce identity quaternion {0,0,0,1} */
  CHECK_VEC4_EQ(q, ((quat){0, 0, 0, 1}), 1e-4f);
}

TEST(quat_from_mat4x4_rotate) {
  quat orig;
  vec3 axis = {0, 1, 0};
  quat_rotate(orig, M_PI / 3.0f, axis);
  mat4x4 M;
  mat4x4_from_quat(M, orig);
  quat recovered;
  quat_from_mat4x4(recovered, M);
  /* Round-trip: quat->matrix->quat should recover original */
  CHECK_VEC4_EQ(recovered, orig, 1e-4f);
}

TEST(quat_conj) {
  quat q, qconj, result;
  vec3 axis = {0, 0, 1};
  quat_rotate(q, M_PI / 2.0f, axis);
  quat_conj(qconj, q);
  /* use separate result var to avoid aliasing output with input */
  quat_mul(result, q, qconj);
  quat id;
  quat_identity(id);
  CHECK_VEC4_EQ(result, id, 1e-4f);
}

TEST(quat_inner_product) {
  quat a, b;
  quat_identity(a);
  quat_identity(b);
  CHECK_FLOAT_EQ(quat_inner_product(a, b), 1.0f, 1e-6f);
}

/* --- Frustum / AABB --- */

TEST(extract_frustum_planes_perspective) {
  mat4x4 proj;
  mat4x4_perspective(proj, M_PI / 2.0f, 1.0f, 0.1f, 100.0f);
  vec4 planes[6];
  mat4x4_extract_frustum_planes(planes, proj);
  for (int i = 0; i < 6; i++) {
    CHECK(isfinite(planes[i][0]) && isfinite(planes[i][1]) &&
          isfinite(planes[i][2]) && isfinite(planes[i][3]));
  }
}

TEST(frustum_test_aabb_inside) {
  mat4x4 proj, view, vp;
  mat4x4_perspective(proj, M_PI / 2.0f, 1.0f, 0.1f, 100.0f);
  vec3 eye = {0, 0, 10}, center = {0, 0, 0}, up = {0, 1, 0};
  mat4x4_look_at(view, eye, center, up);
  mat4x4_mul(vp, view, proj);
  vec4 planes[6];
  mat4x4_extract_frustum_planes(planes, vp);
  vec3 aabb_min = {-0.5f, -0.5f, -0.5f}, aabb_max = {0.5f, 0.5f, 0.5f};
  CHECK(ex_frustum_test_aabb(planes, aabb_min, aabb_max));
}

TEST(frustum_test_aabb_outside) {
  mat4x4 proj, view, vp;
  mat4x4_perspective(proj, M_PI / 2.0f, 1.0f, 0.1f, 100.0f);
  vec3 eye = {0, 0, 10}, center = {0, 0, 0}, up = {0, 1, 0};
  mat4x4_look_at(view, eye, center, up);
  mat4x4_mul(vp, view, proj);
  vec4 planes[6];
  mat4x4_extract_frustum_planes(planes, vp);
  vec3 aabb_min = {100, 100, 100}, aabb_max = {101, 101, 101};
  CHECK(!ex_frustum_test_aabb(planes, aabb_min, aabb_max));
}

TEST(transform_aabb_identity) {
  mat4x4 M;
  mat4x4_identity(M);
  vec3 local_min = {-1, -1, -1}, local_max = {1, 1, 1};
  vec3 world_min, world_max;
  mat4x4_transform_aabb(world_min, world_max, M, local_min, local_max);
  CHECK_VEC3_EQ(world_min, local_min, 1e-6f);
  CHECK_VEC3_EQ(world_max, local_max, 1e-6f);
}

TEST(transform_aabb_translate) {
  mat4x4 M;
  mat4x4_identity(M);
  mat4x4_translate_in_place(M, 5, 0, 0);
  vec3 local_min = {-1, -1, -1}, local_max = {1, 1, 1};
  vec3 world_min, world_max;
  mat4x4_transform_aabb(world_min, world_max, M, local_min, local_max);
  CHECK_VEC3_EQ(world_min, ((vec3){4, -1, -1}), 1e-6f);
  CHECK_VEC3_EQ(world_max, ((vec3){6, 1, 1}), 1e-6f);
}

/* --- Utility --- */

TEST(lerp) {
  CHECK_FLOAT_EQ(lerp(0.0f, 10.0f, 0.0f), 0.0f, 1e-6f);
  CHECK_FLOAT_EQ(lerp(0.0f, 10.0f, 1.0f), 10.0f, 1e-6f);
  CHECK_FLOAT_EQ(lerp(0.0f, 10.0f, 0.5f), 5.0f, 1e-6f);
}

TEST(degrees_radians) {
  CHECK_FLOAT_EQ(degrees(M_PI), 180.0f, 1e-6f);
  CHECK_FLOAT_EQ(rad(180.0f), M_PI, 1e-6f);
  CHECK_FLOAT_EQ(degrees(rad(90.0f)), 90.0f, 1e-6f);
}

TEST(median) {
  CHECK(median(1, 2, 3) == 2.0);
  CHECK(median(3, 1, 2) == 2.0);
  CHECK(median(1, 1, 1) == 1.0);
  CHECK(median(5, 1, 3) == 3.0);
}

TEST(nonzero_sign) {
  CHECK(nonzero_sign(5.0) == 1);
  CHECK(nonzero_sign(-3.0) == -1);
  CHECK(nonzero_sign(0.1) == 1);
}

int main(void) {
  TEST_RUN_INTERNAL(vec3_add);
  TEST_RUN_INTERNAL(vec3_sub);
  TEST_RUN_INTERNAL(vec3_scale);
  TEST_RUN_INTERNAL(vec3_mul_inner);
  TEST_RUN_INTERNAL(vec3_len);
  TEST_RUN_INTERNAL(vec3_norm);
  TEST_RUN_INTERNAL(vec3_norm_zero_vector);
  TEST_RUN_INTERNAL(vec3_min_max);
  TEST_RUN_INTERNAL(vec3_mul_cross);
  TEST_RUN_INTERNAL(vec3_reflect);
  TEST_RUN_INTERNAL(vec3_lerp);
  TEST_RUN_INTERNAL(vec3_div);
  TEST_RUN_INTERNAL(vec3_trim_to_shorter);
  TEST_RUN_INTERNAL(vec3_clamp);
  TEST_RUN_INTERNAL(vec2_len);
  TEST_RUN_INTERNAL(vec2_cross);
  TEST_RUN_INTERNAL(vec4_mul_cross);
  TEST_RUN_INTERNAL(mat4x4_identity);
  TEST_RUN_INTERNAL(mat4x4_dup);
  TEST_RUN_INTERNAL(mat4x4_transpose);
  TEST_RUN_INTERNAL(mat4x4_mul_identity);
  TEST_RUN_INTERNAL(mat4x4_mul_known);
  TEST_RUN_INTERNAL(mat4x4_mul_vec4);
  TEST_RUN_INTERNAL(mat4x4_translate_mul_vec4);
  TEST_RUN_INTERNAL(mat4x4_translate_in_place);
  TEST_RUN_INTERNAL(mat4x4_scale_xyz);
  TEST_RUN_INTERNAL(mat4x4_invert_identity);
  TEST_RUN_INTERNAL(mat4x4_invert_roundtrip);
  TEST_RUN_INTERNAL(mat4x4_invert_scale);
  TEST_RUN_INTERNAL(mat4x4_rotate_X_quarter);
  TEST_RUN_INTERNAL(mat4x4_rotate_Y_quarter);
  TEST_RUN_INTERNAL(mat4x4_rotate_Z_quarter);
  TEST_RUN_INTERNAL(mat4x4_rotate_axis_angle);
  TEST_RUN_INTERNAL(mat4x4_rotate_zero_axis);
  TEST_RUN_INTERNAL(mat4x4_frustum);
  TEST_RUN_INTERNAL(mat4x4_ortho);
  TEST_RUN_INTERNAL(mat4x4_perspective);
  TEST_RUN_INTERNAL(mat4x4_look_at);
  TEST_RUN_INTERNAL(mat4x4_look_at_no_crash_parallel_up);
  TEST_RUN_INTERNAL(mat4x4_orthonormalize);
  TEST_RUN_INTERNAL(quat_identity);
  TEST_RUN_INTERNAL(quat_identity_rotate_vec3);
  TEST_RUN_INTERNAL(quat_mul_vec3_180);
  TEST_RUN_INTERNAL(quat_mul_vec3_90_y);
  TEST_RUN_INTERNAL(quat_slerp_weight_zero);
  TEST_RUN_INTERNAL(quat_slerp_weight_one);
  TEST_RUN_INTERNAL(quat_slerp_midpoint);
  TEST_RUN_INTERNAL(quat_slerp_opposite);
  TEST_RUN_INTERNAL(quat_slerp_near_parallel);
  TEST_RUN_INTERNAL(quat_mul);
  TEST_RUN_INTERNAL(quat_from_mat4x4_identity_roundtrip);
  TEST_RUN_INTERNAL(quat_from_mat4x4_rotate);
  TEST_RUN_INTERNAL(quat_conj);
  TEST_RUN_INTERNAL(quat_inner_product);
  TEST_RUN_INTERNAL(extract_frustum_planes_perspective);
  TEST_RUN_INTERNAL(frustum_test_aabb_inside);
  TEST_RUN_INTERNAL(frustum_test_aabb_outside);
  TEST_RUN_INTERNAL(transform_aabb_identity);
  TEST_RUN_INTERNAL(transform_aabb_translate);
  TEST_RUN_INTERNAL(lerp);
  TEST_RUN_INTERNAL(degrees_radians);
  TEST_RUN_INTERNAL(median);
  TEST_RUN_INTERNAL(nonzero_sign);

  printf("\n" TEST_GREEN "%d passed" TEST_RESET ", " TEST_RED "%d failed"
         TEST_RESET "\n",
         _test_pass, _test_fail);
  return _test_fail ? 1 : 0;
}
