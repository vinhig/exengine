#pragma once

typedef enum cvar_id_e {
  CVAR_SSAO_ENABLED,
  CVAR_FULLSCREEN,
  CVAR_VSYNC,
  CVAR_SCREEN_WIDTH,
  CVAR_SCREEN_HEIGHT,
  CVAR_SSAO_NUM_SAMPLES,
} cvar_id_t;

typedef struct cvar_s {
  char* category;
  char* name;
  cvar_id_t id;

  // default value to set the value to if nothing is
  // specified in the configuration file
  union {
    float f32;
    int i32;
    char* str;
    bool boolean;
  } default_value;

  // value loaded from the configuration file when registering
  // the cvar (if nothing is specified in the configuration file,
  // this will be set to the "default_value")
  union {
    int i32;
    float f32;
    char* str;
    bool boolean;
  } value;

  enum {
    CVAR_F32,
    CVAR_I32,
    CVAR_STR,
    CVAR_BOOL,
  } value_type;

  void* data_ptr;

  void (*callback)(struct cvar_s* var);
} cvar_t;

void ex_cvar_init();

/**
 * [ex_cvar_register]
 * @param cvar [cvar to register, must have the same lifetime as the game itself]
 */
void ex_cvar_register(cvar_t* cvar);

const cvar_t* ex_cvar_get_by_id(cvar_id_t id);
const cvar_t* ex_cvar_get_by_name(const char* name);
void ex_cvar_destroy();