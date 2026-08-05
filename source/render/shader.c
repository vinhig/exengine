#include "log/log.h"

#include <exengine/render/shader.h>
#include <exengine/util/io.h>

#include <stdlib.h>
#include <string.h>

#define EX_MAX_SHADERS 512
#define EX_MAX_UNIFORMS 256

// map raw GL program IDs onto bounded internal slots so that arbitrary
// program IDs can never index out of bounds
static GLuint ex_uniform_shader_ids[EX_MAX_SHADERS];
static size_t ex_uniform_shader_count = 0;
static GLuint ex_uniform_last_shader = 0;
static size_t ex_uniform_last_slot = 0;

// per-slot uniform location caches, keyed by the djb2 hash of the uniform name
static GLint ex_uniform_map[EX_MAX_SHADERS][EX_MAX_UNIFORMS] = {{0}};
static GLint ex_uniform_locations[EX_MAX_SHADERS][EX_MAX_UNIFORMS] = {{0}};

GLuint active_shader = 0;

static size_t ex_uniform_slot(GLuint shader) {
  if (shader == ex_uniform_last_shader) {
    return ex_uniform_last_slot;
  }

  for (size_t i = 0; i < ex_uniform_shader_count; i++) {
    if (ex_uniform_shader_ids[i] == shader) {
      ex_uniform_last_shader = shader;
      ex_uniform_last_slot = i;
      return i;
    }
  }

  if (ex_uniform_shader_count < EX_MAX_SHADERS) {
    size_t slot = ex_uniform_shader_count++;
    ex_uniform_shader_ids[slot] = shader;
    ex_uniform_last_shader = shader;
    ex_uniform_last_slot = slot;
    return slot;
  }

  // all slots taken, skip caching
  return (size_t)-1;
}

inline GLint ex_uniform(GLuint shader, const char *str) {
  const char *string = str;
  uint32_t key = 5381;
  int c;

  // hash * 33 + c
  while ((c = *str++)) {
    key = ((key << 5) + key) + c;
  }

  size_t slot = ex_uniform_slot(shader);
  if (slot == (size_t)-1) {
    return glGetUniformLocation(shader, string);
  }

  // check if location cached already
  int i = 0;
  for (i = 0; i < EX_MAX_UNIFORMS; i++) {
    // end of array
    if (!ex_uniform_map[slot][i]) {
      break;
    }

    // check cached
    if (ex_uniform_map[slot][i] == key) {
      return ex_uniform_locations[slot][i];
    }
  }

  // store and return it
  GLint value = glGetUniformLocation(shader, string);
  if (i < EX_MAX_UNIFORMS) {
    ex_uniform_map[slot][i] = key;
    ex_uniform_locations[slot][i] = value;
  }

  return value;
}

bool ex_gl_check_compilation(GLuint shader, const char *label) {
  GLchar compile_log[512];
  GLint success;

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, compile_log);
    log_error("Failed to compile %s shader\n%s\n", label, compile_log);
    return false;
  }

  return true;
}

GLuint ex_graphic_pipeline_new(const char *path) {
  char vs_path[2048];
  char fs_path[2048];
  char gs_path[2048];
  snprintf(&vs_path[0], 2048, "%s%s.vs.glsl", EX_SHADER_LOC, path);
  snprintf(&fs_path[0], 2048, "%s%s.fs.glsl", EX_SHADER_LOC, path);
  snprintf(&gs_path[0], 2048, "%s%s.gs.glsl", EX_SHADER_LOC, path);

  bool vs_exists = ex_io_exist(vs_path);
  bool fs_exists = ex_io_exist(fs_path);
  bool gs_exists = ex_io_exist(gs_path);

  if (!vs_exists || !fs_exists) {
    log_error("All graphic pipelines must have a vertex and a fragment shader.\n");
    return 0;
  }

  char *vs_source = ex_io_read(vs_path, "r", nullptr);
  char *fs_source = ex_io_read(fs_path, "r", nullptr);

  GLuint program = glCreateProgram();

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  GLuint gs = 0;

  glShaderSource(vs, 1, (const char *const *)&vs_source, nullptr);
  glShaderSource(fs, 1, (const char *const *)&fs_source, nullptr);

  glCompileShader(vs);
  glCompileShader(fs);

  if (!ex_gl_check_compilation(vs, "vertex") || !ex_gl_check_compilation(fs, "fragment")) {
    free(vs_source);
    free(fs_source);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteProgram(program);

    return 0;
  }

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  if (gs_exists) {
    char *gs_source = ex_io_read(gs_path, "r", nullptr);
    gs = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gs, 1, (const char *const *)&gs_source, nullptr);
    glCompileShader(gs);
    if (!ex_gl_check_compilation(gs, "geometry")) {
      free(vs_source);
      free(fs_source);
      glDeleteShader(vs);
      glDeleteShader(fs);
      glDeleteShader(gs);
      glDeleteProgram(program);
    }
    glAttachShader(program, gs);

    free(gs_source);
  }

  glLinkProgram(program);

  free(vs_source);
  free(fs_source);

  glDeleteShader(vs);
  glDeleteShader(fs);
  if (gs_exists) {
    glDeleteShader(gs);
  }

  return program;
}

void ex_graphic_pipeline_use(GLuint shader) {
  if (active_shader == shader) {
    return;
  }

  glUseProgram(shader);
}