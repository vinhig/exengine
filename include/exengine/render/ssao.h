/* ssao
A half-kernel ssao implementation,
  pretty performance heavy.
*/

#pragma once

#include <exengine/math/mathlib.h>

#include <glad/glad.h>

#define SSAO_NUM_SAMPLES 32

/**
 * @brief Initialize the SSAO module.
 */
void ex_ssao_init();

/**
 * @brief Render SSAO to the current render target.
 * @param projection camera projection matrix
 * @param view       camera view matrix
 * @param gposition  G-buffer position texture
 * @param gnormal    G-buffer normal texture
 * @param fbo_vao    fullscreen quad VAO
 */
void ex_ssao_render(mat4x4* projection, mat4x4* view, GLuint gposition, GLuint gnormal, GLuint fbo_vao);

/**
 * @brief Bind the SSAO occlusion texture.
 * @param shader shader to use
 */
void ex_ssao_bind_texture(GLuint shader);

/**
 * @brief Bind an empty (default) SSAO texture.
 * @param shader shader to use
 */
void ex_ssao_bind_default(GLuint shader);

/**
 * @brief Clean up SSAO resources.
 */
void ex_ssao_destroy();
