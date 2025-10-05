#include "exengine/util/cvar.h"
#include "log/log.h"

#include <exengine/engine.h>

#include <physfs/physfs.h>

#include <exengine/input/input.h>
#include <exengine/render/text.h>
#include <exengine/render/window.h>
#include <exengine/util/cache.h>
#include <exengine/util/ini.h>
#include <sys/stat.h>

// user defined function callback pointers
void (*ex_init_ptr)(void) = nullptr;
void (*ex_update_ptr)(double, double) = nullptr;
void (*ex_draw_ptr)(void) = nullptr;
void (*ex_exit_ptr)(void) = nullptr;
// non-essential user callbacks
void (*ex_keypressed_ptr)(uint32_t) = nullptr;
void (*ex_mousepressed_ptr)(uint8_t) = nullptr;
void (*ex_mousemotion_ptr)(int, int) = nullptr;
void (*ex_mousewheel_ptr)(int32_t, int32_t) = nullptr;
void (*ex_resize_ptr)(uint32_t, uint32_t) = nullptr;
// custom event handling
void (*ex_event_handler)(SDL_Event *) = nullptr;
// allows full override of default event handler
void (*ex_event_handler_full)(SDL_Event *) = nullptr;

cvar_t cvar_screen_width = {
    .category = "graphics",
    .name = "screen_width",
    .id = CVAR_SCREEN_WIDTH,
    .value_type = CVAR_I32,
    .default_value = {.i32 = 1920},
};

cvar_t cvar_screen_height = {
    .category = "graphics",
    .name = "screen_height",
    .id = CVAR_SCREEN_HEIGHT,
    .value_type = CVAR_I32,
    .default_value = {.i32 = 1080},
};

cvar_t cvar_fullscreen = {
    .category = "graphics",
    .name = "fullscreen",
    .id = CVAR_FULLSCREEN,
    .value_type = CVAR_BOOL,
    .default_value = {.boolean = false},
};

cvar_t cvar_vsync = {
    .category = "graphics",
    .name = "vsync",
    .id = CVAR_VSYNC,
    .value_type = CVAR_BOOL,
    .default_value = {.boolean = false},
};

void exengine(char **argv, const char *appname, uint8_t flags) {
  /* -- INIT ENGINE -- */
  // init physfs filesystem
  PHYSFS_init(argv[0]);

  log_set_level(LOG_WARN);

  // set the safe writing dir
  // most often these directories will be..
  // linux: ~/.local/share/appname
  // windows: AppData\\Roaming\\appname\\appname
  const char *write_path = PHYSFS_getPrefDir(appname, appname);
  if (write_path != nullptr) {
    if (!PHYSFS_setWriteDir(write_path)) {
      log_warn("PhysFS was unable to set the write directory!");
    } else {
      // create missing directory
      char data_path[2048];
      sprintf(&data_path[0], "%s/data", write_path);
      mkdir(data_path, 0755);
    }
  } else {
    log_warn("PhysFS was unable to find the write directory!");
  }
  log_info("PHYSFS_getPrefDir returned %s.", write_path);

  // append data and write paths to search paths
  PHYSFS_mount(write_path, nullptr, 1);
  PHYSFS_mount(EX_DATA_FILE, nullptr, 1);

  // init engine file data cache
  ex_cache_init();

  ex_cvar_init();

  ex_cvar_register(&cvar_screen_width);
  ex_cvar_register(&cvar_screen_height);
  ex_cvar_register(&cvar_fullscreen);
  ex_cvar_register(&cvar_vsync);

  // init subsystems
  if (flags & EX_ENGINE_SOUND) {
    ex_sound_init();
  }

  // delta time vars
  const double phys_delta_time = 1.0 / 60.0;
  const double slowest_frame = 1.0 / 15.0;
  double delta_time, accumulator = 0.0;

  // load config vars
  uint32_t width = cvar_screen_width.value.i32;
  uint32_t height = cvar_screen_height.value.i32;

  // init the window and gl
  if (!ex_window_init(width, height, "exengine-testing")) {
    ex_exit_ptr();
    return;
  }

  // init rendering modules
  ex_font_init();

  // user init callback
  ex_init_ptr();
  /* ----------------- */

  /* -- UPDATE ENGINE -- */
  // main engine loop
  double last_ex_frame_time = (double)SDL_GetPerformanceCounter();
  int running = 1;

  while (running) {
    // calculate delta time
    double current_ex_frame_time = (double)SDL_GetPerformanceCounter();
    delta_time = (double)(current_ex_frame_time - last_ex_frame_time) / (double)SDL_GetPerformanceFrequency();
    last_ex_frame_time = current_ex_frame_time;

    // prevent spiral of death
    if (delta_time > slowest_frame) {
      delta_time = slowest_frame;
    }

    // update at a constant rate to keep physics in check
    accumulator += delta_time;
    while (accumulator >= phys_delta_time) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        // full event handler override
        if (ex_event_handler_full) {
          ex_event_handler_full(&event);
          continue;
        }

        // custom event handler
        // doesnt override anything
        if (ex_event_handler) {
          ex_event_handler(&event);
        }

        // default event handler
        switch (event.type) {
        // cya
        case SDL_EVENT_QUIT:
          running = 0;
          break;

        // input events
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_WHEEL:
        case SDL_EVENT_TEXT_EDITING:
        case SDL_EVENT_TEXT_INPUT:
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_KEYMAP_CHANGED: {
          ex_input_event(&event);
          break;
        }

        // window events
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_RESIZED: {
          ex_window_event(&event);
          break;
        }
        }
      }

      // handle other frequent input updates
      ex_input_update();

      // user update callback
      ex_update_ptr(phys_delta_time, delta_time);

      accumulator -= phys_delta_time;
    }

    // user draw callback
    ex_draw_ptr();

    // swap buffers
    SDL_GL_SwapWindow(display.window);
  }
  /* ------------------- */

  // -- CLEAN UP -- */
  ex_cvar_destroy();
  ex_window_destroy();
  PHYSFS_deinit();
  ex_cache_flush();
  if (flags & EX_ENGINE_SOUND) {
    ex_sound_exit();
  }

  // user exit callback
  ex_exit_ptr();
  // -------------- */
}