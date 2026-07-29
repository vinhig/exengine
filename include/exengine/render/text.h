/* text
  Handles text and font rendering using
  multi-channel signed distance fields (MSDF).
 */

#pragma once

#include "glad/glad.h"

#include "msdf.h"

#define MAX_GLYPH 512

typedef struct {
  GLuint texture;
  char indices[MAX_GLYPH];
  ex_metrics_t *metrics;
  float *uv;
} ex_font_t;

/**
 * @brief Initialize the font module.
 */
void ex_font_init();

/**
 * @brief Load a font from a TrueType file.
 * @param path    path to the font file
 * @param letters string of characters to generate glyphs for
 * @return pointer to the new font, or NULL on failure
 */
ex_font_t *ex_font_load(const char *path, const char *letters);

/**
 * @brief Debug print font information.
 * @param f the font to print
 */
void ex_font_dbg(ex_font_t *f);

/**
 * @brief Clean up font data.
 * @param f the font to destroy
 */
void ex_font_destroy(ex_font_t *f);
