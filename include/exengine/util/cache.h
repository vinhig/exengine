/* cache
  Handles loading and caching most generic types
  of data.

  Once something has been loaded once, it will
  be kept in cache and future references to
  that file will instead return a copy of the
  cached data.

  Note that most data loading functions (like
  the iqm loader) call these internally already.
*/

#pragma once

#include "glad/glad.h"

#include "../render/model.h"

/**
 * @brief Initialize the cache module.
 */
void ex_cache_init();

/**
 * @brief Store a model in the cache.
 * @param m model to add
 */
void ex_cache_model(ex_model_t *m);

/**
 * @brief Get a copy of a model if it exists in the cache.
 * @param path path to the model file
 * @return a copy of the requested model, or NULL
 */
ex_model_t *ex_cache_get_model(const char *path);

/**
 * @brief Store or retrieve a cached texture.
 * @param path path to the texture file
 * @return the cached texture GLuint
 */
GLuint ex_cache_texture(const char *path);

/**
 * @brief Clean up all data from the cache.
 */
void ex_cache_flush();
