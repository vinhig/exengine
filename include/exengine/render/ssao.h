/* ssao
A half-kernel ssao implementation,
  pretty performance heavy.
*/

#pragma once

#include <exengine/math/mathlib.h>

#include <glad/glad.h>

#define SSAO_NUM_SAMPLES 32

/**
 * [ex_ssao_init init the ssao module]
 */
void ex_ssao_init();

/**
 * [ex_ssao_render set as render target]
 * @param projection [cameras projection matrix]
 * @param view       [cameras view matrix]
 */
void ex_ssao_render(mat4x4 projection, mat4x4 view, GLuint gposition, GLuint gnormal, GLuint fbo_vao);

/**
 * [ex_ssao_bind_texture]
 * @param shader [shader to use]
 */
void ex_ssao_bind_texture(GLuint shader);

/**
 * [ssao_bind_default binds an empty ssao texture]
 * @param shader [shader to use]
 */
void ex_ssao_bind_default(GLuint shader);
