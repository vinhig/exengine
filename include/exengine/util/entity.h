/* entity
  The 3d entity component of the engine,
  defined as an ellipsoid that slides around
  the game level.

  Part of the response step (as well as detection step)
  is based on the following paper by Kasper Fauerby.
  http://www.peroxide.dk/papers/collision/collision.pdf

  A more robust response step has been implemented
  based on the follow-up paper to the above by
  Jeff Linahan.
  https://arxiv.org/ftp/arxiv/papers/1211/1211.0059.pdf
*/

#pragma once

#include <exengine/math/collision.h>
#include <exengine/math/mathlib.h>
#include <exengine/render/scene.h>

typedef struct {
  vec3 position, velocity, radius;
  ex_coll_packet_t packet;
  ex_scene_t *scene;
  int grounded;
} ex_entity_t;

/**
 * @brief Create a new entity (ellipsoid) in a scene.
 * @param scene  the scene the entity will reside in
 * @param radius the radius of the ellipsoid
 * @return pointer to the new entity
 */
ex_entity_t *ex_entity_new(ex_scene_t *scene, vec3 radius);

/**
 * @brief Perform collision detection and sliding response.
 * @param entity entity to update
 */
void ex_entity_collide_and_slide(ex_entity_t *entity);

/**
 * @brief Check collision between an entity and the world.
 * @param entity     entity to update
 * @param e_position initial position in ellipsoid space
 * @param e_velocity initial velocity in ellipsoid space
 */
void ex_entity_collide_with_world(ex_entity_t *entity, vec3 e_position, vec3 e_velocity);

/**
 * @brief Check collision against the scene octree.
 * @param entity entity to check
 */
void ex_entity_check_collision(ex_entity_t *entity);

/**
 * @brief Check if the entity is standing on the ground.
 * @param entity entity to check
 */
void ex_entity_check_grounded(ex_entity_t *entity);

/**
 * @brief Update an entity, calling collision functions.
 * @param entity entity to update
 * @param dt     delta time
 */
void ex_entity_update(ex_entity_t *entity, double dt);

/**
 * @brief Cast a ray from one point to another and find the nearest intersection.
 * @param entity entity to use for collision parameters
 * @param from   ray origin
 * @param to     ray destination
 * @param plane  output intersection plane
 * @return the fraction of the ray traveled before intersection
 */
float ex_raycast(const ex_entity_t *entity, vec3 from, vec3 to, ex_plane_t *plane);
