/* shader
  Loads and compiles shaders.

  Requires at minimal a vertex and
  fragment shader, can also compile a
  geometry shader if specified.
*/

#pragma once

#define EX_SHADER_LOC "data/shaders/"

#include "glad/glad.h"

typedef struct {
  GLuint ID;
  char path[512];
} ex_shader_t;

/**
 * @brief Cache and return shader uniform locations.
 * @param shader shader to be used
 * @param str    uniform name string
 * @return uniform location
 *
 * Hashes the string key using djb2.
 * www.cse.yorku.ca/~oz/hash.html
 */
GLint ex_uniform(GLuint shader, const char *str);

/**
 * @brief Load, attach, and link shaders into a shader program.
 * @param path shader file path
 * @return the shader program GLuint
 *
 * Requires at minimum a vertex and fragment shader file.
 * Move this to a .c file, it should not be in the header.
 */

GLuint ex_graphic_pipeline_new(const char *path);

/**
 * @brief Bind a shader for use, assuming it is not already in use.
 * @param shader shader to use
 */
void ex_graphic_pipeline_use(GLuint shader);
