/* scene
  The scene handler, where the magic happens.

  This contains everything you
  want to render each frame, as
  well as your collision vertices
  which are stored in an internal
  octree.

  Currently, it uses a deferred renderer,
  and has semi-function light culling.

  The following are features that need implementing:
  * Frustum culling for models, lights etc
*/

#pragma once

#include <exengine/math/octree.h>
#include <exengine/render/renderer.h>
#include <exengine/util/list.h>

#define EX_MAX_POINT_LIGHTS 500
#define EX_MAX_SPOT_LIGHTS 128
#define EX_MAX_REFLECTIONS 50

// this should be the value of the biggest define above
#define EX_SCENE_BIGGEST_LIGHT 500

#define EX_SCENE_MAX_MODELS 500

typedef struct ex_scene_t {
  vec3 gravity;

  ex_list_t *coll_list;
  ex_octree_t *coll_tree;
  int collision_built;
  vec3 *coll_vertices;
  size_t coll_vertices_last;

  ex_renderable_t renderables;
} ex_scene_t;

/**
 * @brief Create a new scene.
 * @param flags the features to enable
 * @return pointer to the new scene
 */
ex_scene_t *ex_scene_new(uint8_t flags);

/**
 * @brief Add a model's vertices to the collision tree.
 * @param s the scene to use
 * @param m the model which contains the vertices
 *
 * This could do with being more generic
 * and being moved away from models.
 */
void ex_scene_add_collision(ex_scene_t *s, ex_model_t *m);

/**
 * @brief Build the collision tree.
 * @param s the scene to use
 */
void ex_scene_build_collision(ex_scene_t *s);

/**
 * @brief Add a model to the render list.
 * @param s the scene
 * @param m the model to add
 */
void ex_scene_add_model(ex_scene_t *s, ex_model_t *m);

/**
 * @brief Remove a model from the render list.
 * @param s the scene
 * @param m the model to remove
 */
void ex_scene_remove_model(ex_scene_t *s, ex_model_t *m);

/**
 * @brief Add a point light to the scene.
 * @param s  the scene to use
 * @param pl the point light to add
 */
void ex_scene_add_pointlight(ex_scene_t *s, ex_point_light_t *pl);

/**
 * @brief Update the scene (collision, models, etc.).
 * @param s          the scene to use
 * @param delta_time time since last update in seconds
 */
void ex_scene_update(ex_scene_t *s, float delta_time);

/**
 * @brief Render the scene.
 * @param s        the scene to render
 * @param matrices the camera matrices
 */
void ex_scene_draw(ex_scene_t *s, ex_camera_matrices_t *matrices);

/**
 * @brief Resize internal framebuffers.
 * @param s      the scene to resize
 * @param width  new width
 * @param height new height
 */
void ex_scene_resize(ex_scene_t *s, int width, int height);

/**
 * @brief Clean up scene data.
 * @param s the scene to destroy
 */
void ex_scene_destroy(ex_scene_t *s);
