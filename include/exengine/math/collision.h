/* collision
  Handles the collision checking between
  an ellipsoid and a triangle.

  Based on Kasper Fauerbys's paper.
  http://www.peroxide.dk/papers/collision/collision.pdf
*/

#pragma once

#include <exengine/math/mathlib.h>

typedef struct {
  vec3 origin;
  vec3 normal;
  float equation[4];
  vec3 intersection_point;
} ex_plane_t;

typedef struct {
  // r3 space
  vec3 r3_velocity, r3_position;

  // ellipsoid space
  vec3 e_radius;
  vec3 e_velocity;
  vec3 e_norm_velocity;
  vec3 e_base_point;

  // original tri points
  vec3 a, b, c;

  // hit information
  int found_collision;
  float nearest_distance;
  double t;
  vec3 intersect_point;
  ex_plane_t plane;

  // iteration depth
  int depth;
} ex_coll_packet_t;

/**
 * @brief Define a plane from an origin and normal.
 * @param a plane origin
 * @param b plane normal
 * @return the generated plane
 */
ex_plane_t ex_plane_new(const vec3 a, const vec3 b);

/**
 * @brief Define a plane from a triangle.
 * @param a triangle vertex a
 * @param b triangle vertex b
 * @param c triangle vertex c
 * @return the generated plane
 */
ex_plane_t ex_triangle_to_plane(const vec3 a, const vec3 b, const vec3 c);

/**
 * @brief Compute the signed distance from a point to a plane.
 * @param base_point origin point
 * @param plane      the plane
 * @return the signed distance
 */
float ex_signed_distance_to_plane(const vec3 base_point, const ex_plane_t *plane);

/**
 * @brief Check if a plane is front-facing relative to a direction.
 * @param plane     the plane to check
 * @param direction the direction vector
 * @return 1 if front-facing
 */
int ex_is_front_facing(ex_plane_t *plane, const vec3 direction);

/**
 * @brief Check if a point lies inside a triangle.
 * @param point the point to check
 * @param p1    triangle vertex a
 * @param p2    triangle vertex b
 * @param p3    triangle vertex c
 * @return 1 if point resides in the triangle
 */
int ex_check_point_in_triangle(const vec3 point, const vec3 p1, const vec3 p2, const vec3 p3);

/**
 * @brief Find the lowest root of a quadratic equation.
 * @param a    quadratic coefficient
 * @param b    linear coefficient
 * @param c    constant coefficient
 * @param max  maximum allowed root value
 * @param root output pointer for the root
 * @return 1 if a solution exists
 */
int ex_get_lowest_root(float a, float b, float c, float max, float *root);

/**
 * @brief Ray-triangle intersection using the Möller–Trumbore algorithm.
 * @param from      ray origin
 * @param to        ray direction
 * @param v0        triangle vertex 0
 * @param v1        triangle vertex 1
 * @param v2        triangle vertex 2
 * @param intersect output intersection point
 * @return 1 if intersection occurs, 0 otherwise
 */
int ray_in_tri(vec3 from, vec3 to, vec3 v0, vec3 v1, vec3 v2, vec3 intersect);

/**
 * @brief Main collision iteration between an ellipsoid and a triangle.
 * @param packet the collision packet
 * @param p1     triangle vertex a
 * @param p2     triangle vertex b
 * @param p3     triangle vertex c
 */
void ex_collision_check_triangle(ex_coll_packet_t *packet, const vec3 p1, const vec3 p2, const vec3 p3);
