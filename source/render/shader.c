#include "log/log.h"

#include <exengine/render/shader.h>
#include <exengine/util/io.h>

#include <stdlib.h>
#include <string.h>

#define EX_MAX_SHADERS 512

GLint ex_uniform_map[256][256] = {{0}};
GLint ex_uniform_locations[256][256] = {{0}};

ex_shader_t shader_list[EX_MAX_SHADERS];
size_t shader_count = 0;

GLuint active_shader = 0;

inline GLint ex_uniform(GLuint shader, const char *str) {
  const char *string = str;
  uint32_t key = 5381;
  int c;

  // hash * 33 + c
  while ((c = *str++)) {
    key = ((key << 5) + key) + c;
  }

  // check if location cached already
  int i = 0;
  for (i = 0; i < 256; i++) {
    // end of array
    if (!ex_uniform_map[shader][i]) {
      break;
    }

    // check cached
    if (ex_uniform_map[shader][i] == key) {
      return ex_uniform_locations[shader][i];
    }
  }

  // store and return it
  GLint value = glGetUniformLocation(shader, string);
  ex_uniform_map[shader][i] = key;
  ex_uniform_locations[shader][i] = value;

  return value;
}

bool ex_gl_check_compilation(GLuint shader, const char* label) {
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

  auto program = glCreateProgram();

  auto vs = glCreateShader(GL_VERTEX_SHADER);
  auto fs = glCreateShader(GL_FRAGMENT_SHADER);
  auto gs = 0;

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