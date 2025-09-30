/* texture
  Handles loading and converting
  a texture file to a GL texture.

  Currently, uses textures with 4
  components. (rgba)
*/

#pragma once

#define EX_TEXTURE_LOC "data/textures/"

#include <stdint.h>

#include "glad/glad.h"

typedef struct {
  GLuint id;
  int width, height;
  char name[33];
  uint8_t *data;
} ex_texture_t;

/**
 * [ex_texture_load load a texture from file]
 * @param  file [file path string]
 * @return      [texture var]
 */
ex_texture_t *ex_texture_load(const char *file, int get_data);
