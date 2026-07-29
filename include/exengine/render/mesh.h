/* mesh
  A generic mesh container.

  Each mesh has a single set of
  textures applied to it, models
  contain a mesh per texture set used.

  This is automatically handled by the IQM
  loader, only use the following if you
  want to manually define vertices
  via code.
*/

#pragma once

#include <string.h>

#include "glad/glad.h"

typedef struct {
  float position[3];
  float uv[2];
  float normal[3];
  float tangent[4];
  uint8_t color[4];
  uint8_t blend_indexes[4];
  uint8_t blend_weights[4];
} ex_vertex_t;

typedef struct {
  GLuint VAO, VBO, EBO, vcount, icount;
  GLuint texture, texture_spec, texture_norm;
  uint32_t current_frame;
  uint8_t is_copy;
} ex_mesh_t;

/**
 * @brief Generate a mesh with the given vertices, indices, and texture.
 * @param vertices pointer to vertices
 * @param vcount   length of vertices
 * @param indices  pointer to indices
 * @param icount   length of indices
 * @param texture  the texture GLuint
 * @return pointer to the new mesh
 */
ex_mesh_t *ex_mesh_new(ex_vertex_t *vertices, size_t vcount, GLuint *indices, size_t icount, GLuint texture);

/**
 * @brief Duplicate a mesh.
 * @param mesh the mesh to copy
 * @return pointer to the new copy
 */
ex_mesh_t *ex_mesh_copy(ex_mesh_t *mesh);

/**
 * @brief Free any allocated mesh data.
 * @param m pointer to the mesh to destroy
 */
void ex_mesh_destroy(ex_mesh_t *m);
