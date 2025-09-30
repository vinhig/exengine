#include "exengine/util/ini.h"
#include "log/log.h"

#include <exengine/util/cvar.h>
#include <stdint.h>
#include <string.h>

cvar_t *cvars[256] = {};
size_t cvar_count = 0;

ex_ini_t configuration = {};

void ex_cvar_init() {
  ex_ini_load_dont_use(&configuration, "data/conf.ini");
}

void ex_cvar_register(cvar_t *cvar) {
  // register (append it to the list)
  cvars[cvar_count] = cvar;
  cvar_count++;

  // check if value is present in conf.ini
  switch (cvar->value_type) {
  case CVAR_F32: {
    if (ex_ini_exists(&configuration, cvar->category, cvar->name)) {
      auto value = ex_ini_get_float(&configuration, cvar->category, cvar->name);
      cvar->value.f32 = value;
    } else {
      cvar->value.f32 = cvar->default_value.f32;
      ex_ini_set_float(&configuration, cvar->category, cvar->name, cvar->default_value.f32);
    }

    break;
  }
  case CVAR_I32: {
    if (ex_ini_exists(&configuration, cvar->category, cvar->name)) {
      auto value = (int32_t)ex_ini_get_float(&configuration, cvar->category, cvar->name);
      cvar->value.i32 = value;
    } else {
      cvar->value.i32 = cvar->default_value.i32;
      ex_ini_set_float(&configuration, cvar->category, cvar->name, cvar->default_value.i32);
    }

    break;
  }
  case CVAR_STR: {
    if (ex_ini_exists(&configuration, cvar->category, cvar->name)) {
      auto value = ex_ini_get_string(&configuration, cvar->category, cvar->name);
      cvar->value.str = value;
    } else {
      cvar->value.str = cvar->default_value.str;
      ex_ini_set_string(&configuration, cvar->category, cvar->name, cvar->default_value.str);
    }

    break;
  }
  case CVAR_BOOL: {
    if (ex_ini_exists(&configuration, cvar->category, cvar->name)) {
      auto value = ex_ini_get_bool(&configuration, cvar->category, cvar->name);
      cvar->value.boolean = value;
    } else {
      cvar->value.boolean = cvar->default_value.boolean;
      ex_ini_set_bool(&configuration, cvar->category, cvar->name, cvar->default_value.boolean);
    }

    break;
  }
  }
}

const cvar_t *ex_cvar_get_by_id(cvar_id_t id) {
  for (size_t i = 0; i < cvar_count; i++) {
    if (cvars[i]->id == id) {
      return cvars[i];
    }
  }

  log_warn("ex_cvar_get_by_id(%d) returned nullptr.", id);
  return nullptr;
}

const cvar_t *ex_cvar_get_by_name(const char *name) {
  for (size_t i = 0; i < cvar_count; i++) {
    if (strcmp(cvars[i]->name, name) == 0) {
      return cvars[i];
    }
  }

  log_warn("ex_cvar_get_by_name(%s) returned nullptr.", name);
  return nullptr;
}

void ex_cvar_destroy() {
  ex_ini_save(&configuration, "data/conf.ini");
}
