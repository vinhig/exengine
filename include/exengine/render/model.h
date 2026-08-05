/* model
  The model container consists of
  multiple meshes, one per texture set.

  It handles skeletal animation, as
  well as the transforms for all of
  its meshes.
*/

#pragma once

#include "glad/glad.h"

#include <exengine/math/mathlib.h>
#include <exengine/render/mesh.h>

#define EX_MODEL_MAX_MESHES 128

typedef struct {
  char name[65];
  int parent;
  vec3 position, scale;
  quat rotation;
  mat4x4 transform;
} ex_bone_t;

typedef struct {
  char *name;
  uint32_t first, last;
  float rate;
  uint8_t loop;
} ex_anim_t;

typedef struct {
  vec3 translate, scale;
  quat rotate;
} ex_pose_t;

typedef ex_pose_t *ex_frame_t;

typedef struct {
  vec3 position, rotation;
  float scale;
} ex_transform_t;

typedef struct {
  ex_mesh_t *meshes[EX_MODEL_MAX_MESHES];

  ex_transform_t transform;
  uint8_t is_lit, cast_shadow;

  ex_anim_t *current_anim;
  float current_time;
  uint32_t current_frame;

  mat4x4 *inverse_base, *skeleton;
  ex_bone_t *bones;
  ex_anim_t *anims;
  ex_frame_t *frames, bind_pose, pose;
  size_t bones_len, anims_len, frames_len;

  vec3 *vertices;
  size_t num_vertices;

  vec3 aabb_min, aabb_max;

  /**
   * All transforms for all possible instances of this model.
   * `transform_matrices` will contain the model matrix for each model
   * that is actually visible. Therefore, `instance_count` is always equal
   * or greater to `visible_instance_count`.
   */
  ex_transform_t *transform_fulls;
  mat4x4 *transform_matrices;
  GLuint instance_vbo;
  size_t instance_count;
  size_t visible_instance_count;

  char path[512];

  uint8_t is_copy;
} ex_model_t;

/**
 * @brief Create a new, empty model.
 * @return pointer to the new model
 */
ex_model_t *ex_model_new();

/**
 * @brief Create a copy that shares the same underlying data.
 * @param model the model to copy
 * @return pointer to the new model instance
 */
ex_model_t *ex_model_copy(ex_model_t *model);

/**
 * @brief Add a mesh to the model's render list.
 * @param m    the model
 * @param mesh mesh to add
 */
void ex_model_add_mesh(ex_model_t *m, ex_mesh_t *mesh);

/**
 * @brief Initialize the instancing transform arrays.
 * @param m     the model to instance
 * @param count how many instances to allocate
 */
void ex_model_init_instancing(ex_model_t *m, int count);

/**
 * @brief Update the model's animations, transforms, etc.
 * @param m          the model to update
 * @param delta_time time since last update in seconds
 */
void ex_model_update(ex_model_t *m, float delta_time);

/**
 * @brief Clean up model data.
 * @param m the model to destroy
 */
void ex_model_destroy(ex_model_t *m);

/**
 * @brief Update bone matrices.
 * @param m pointer to the model
 */
void ex_model_update_matrices(ex_model_t *m);

/**
 * @brief Set the skeleton pose.
 * @param m     pointer to the model
 * @param frame frame data to use as the pose
 */
void ex_model_set_pose(ex_model_t *m, ex_frame_t frame);

/**
 * @brief Set the active animation by name.
 * @param m  pointer to the model
 * @param id animation name
 */
void ex_model_set_anim(ex_model_t *m, char *id);

/**
 * @brief Compute a bone matrix from position, rotation, and scale.
 * @param m     output bone matrix
 * @param pos   position vector
 * @param rot   rotation quaternion
 * @param scale scale vector
 */
void ex_calc_bone_matrix(mat4x4 m, vec3 pos, quat rot, vec3 scale);

/**
 * @brief Interpolate between two animation frames.
 * @param m      pointer to the model
 * @param a      source frame
 * @param b      target frame
 * @param weight blend weight (0.0 to 1.0)
 */
void ex_mix_pose(ex_model_t *m, ex_frame_t a, ex_frame_t b, float weight);
