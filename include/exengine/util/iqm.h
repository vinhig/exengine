/* iqm
  Loads IQM files and defines a model
  from its data.

  This supports all IQM features, including
  skeleton animation.  It also internally
  uses the cache system.
*/

#pragma once

#include <exengine/render/model.h>
#include <exengine/render/scene.h>

#include <stdint.h>

#define EX_IQM_MAGIC "INTERQUAKEMODEL"
#define EX_IQM_VERSION 2

/*
  The loader flags, OR these against
  eachother in the flags argument to
  specify what you want the loader
  to do with the model file.

  if EX_KEEP_VERTICES is defined,
  the loader will add the model vertices
  to the scenes collision tree.
*/
#define EX_KEEP_VERTICES 1

typedef struct {
  char magic[16];
  uint32_t version;
  uint32_t filesize;
  uint32_t flags;
  uint32_t num_text, ofs_text;
  uint32_t num_meshes, ofs_meshes;
  uint32_t num_vertexarrays, num_vertexes, ofs_vertexarrays;
  uint32_t num_triangles, ofs_triangles, ofs_adjacency;
  uint32_t num_joints, ofs_joints;
  uint32_t num_poses, ofs_poses;
  uint32_t num_anims, ofs_anims;
  uint32_t num_frames, num_framechannels, ofs_frames, ofs_bounds;
  uint32_t num_comment, ofs_comment;
  uint32_t num_extensions, ofs_extensions;
} ex_iqm_header_t;

enum {
  IQM_POSITION = 0,
  IQM_TEXCOORD = 1,
  IQM_NORMAL = 2,
  IQM_TANGENT = 3,
  IQM_BLENDINDEXES = 4,
  IQM_BLENDWEIGHTS = 5,
  IQM_COLOR = 6,
  IQM_CUSTOM = 0x10,
  IQM_BYTE = 0,
  IQM_UBYTE = 1,
  IQM_SHORT = 2,
  IQM_USHORT = 3,
  IQM_INT = 4,
  IQM_UINT = 5,
  IQM_HALF = 6,
  IQM_FLOAT = 7,
  IQM_DOUBLE = 8,
  IQM_LOOP = 1 << 0
};

typedef struct {
  uint32_t triangle[3];
} ex_iqmadjacency_t;

typedef struct {
  uint32_t name;
  int parent;
  float translate[3], rotate[4], scale[3];
} ex_iqmjoint_t;

typedef struct {
  int parent;
  uint32_t channelmask;
  float channeloffset[10];
  float channelscale[10];
} ex_iqmex_pose_t;

typedef struct {
  uint32_t name;
  uint32_t first_frame, num_frames;
  float framerate;
  uint32_t flags;
} ex_iqmex_anim_t;

typedef struct {
  uint32_t type;
  uint32_t flags;
  uint32_t format;
  uint32_t size;
  uint32_t offset;
} ex_iqmvertexarray_t;

typedef struct {
  float bbmin[3], bbmax[3];
  float xyradius, radius;
} ex_iqmbounds_t;

typedef struct {
  uint32_t name;
  uint32_t material;
  uint32_t first_vertex, num_vertexes;
  uint32_t first_triangle, num_triangles;
} ex_iqmex_mesh_t;

/**
 * @brief Load an IQM model file.
 * @param scene required if EX_KEEP_VERTICES is set in flags
 * @param path  path to the model file
 * @param flags bitmask of loader flags (see EX_KEEP_VERTICES)
 * @return an instance of the requested IQM model
 */
ex_model_t *ex_iqm_load_model(ex_scene_t *scene, const char *path, uint8_t flags);

/**
 * @brief Read a 32-bit unsigned integer from raw data (little-endian).
 * @param data pointer to raw byte data
 * @return the decoded uint32_t
 */
uint32_t ex_get_uint(const uint8_t *data);

/**
 * @brief Parse up to 4 float arguments from a string.
 * @param str  input string
 * @param args output vec4 of parsed values
 */
void ex_iqm_get_args(const char *str, vec4 args);
