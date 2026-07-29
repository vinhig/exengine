/* camera
  Handles creating and updating various types
  of cameras.
*/

#pragma once

#include <exengine/math/mathlib.h>
#include <exengine/render/model.h>

typedef struct {
  mat4x4 view, projection, inverse_view, frustum, projection_frustum /* projection frustum is slightly larger to avoid pop-in effect */;
} ex_camera_matrices_t;

typedef struct {
  vec3 position, front, up;
  float yaw, pitch, last_x, last_y, fov, sensitivity;
  int width, height, update;
  ex_model_t *view_model;
  ex_camera_matrices_t matrices;
} ex_fps_camera_t;

/**
 * @brief Create a first person camera.
 * @param x           x position
 * @param y           y position
 * @param z           z position
 * @param sensitivity mouse sensitivity
 * @param fov         field of view in radians
 * @return pointer to the new camera
 */
ex_fps_camera_t *ex_fps_camera_new(float x, float y, float z, float sensitivity, float fov);

/**
 * @brief Adjust the projection matrices.
 * @param cam camera to resize
 *
 * To be called right after scene_draw, but only if
 * the specified scene width and height where 0x0.
 */
void ex_fps_camera_resize(ex_fps_camera_t *cam);

/**
 * @brief Handle input and rotation.
 * @param cam             camera to update
 * @param update_rotation whether to update rotation from mouse input
 */
void ex_fps_camera_update(ex_fps_camera_t *cam, bool update_rotation);

/**
 * @brief Clean up camera data.
 * @param cam camera to destroy
 */
void ex_fps_camera_destroy(ex_fps_camera_t *cam);
