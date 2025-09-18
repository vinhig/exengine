/* text
 */

#pragma once

#include "glad/glad.h"

#include "render/msdf.h"

#define MAX_GLYPH 512

typedef struct {
  GLuint texture;
  char indices[MAX_GLYPH];
  ex_metrics_t *metrics;
  float *uv;
} ex_font_t;

void ex_font_init();

ex_font_t *ex_font_load(const char *path, const char *letters);

void ex_font_dbg(ex_font_t *f);
