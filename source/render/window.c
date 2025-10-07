#include "log/log.h"

#include <exengine/engine.h>
#include <exengine/render/window.h>
#include <exengine/util/cvar.h>
#include <glad/glad.h>
#include <stdio.h>

ex_window_t display;

extern cvar_t cvar_screen_width;
extern cvar_t cvar_screen_height;
extern cvar_t cvar_fullscreen;
extern cvar_t cvar_vsync;

int ex_window_init(uint32_t width, uint32_t height, const char *title) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
    log_error("Failed to init SDL. SDL error is -> \n%s", SDL_GetError());
    return 0;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

  // create a window
  display.window = SDL_CreateWindow(title, (int32_t)width, (int32_t)height, SDL_WINDOW_OPENGL);

  if (!display.window) {
    log_error("Failed to open SDL window\n%s", SDL_GetError());
    return 0;
  }

  if (cvar_fullscreen.value.boolean) {
    SDL_SetWindowFullscreen(display.window, true);
  } else {
    SDL_SetWindowFullscreen(display.window, false);
  }

  // attempt to setup GL
  SDL_GL_MakeCurrent(display.window, display.context);
  display.context = SDL_GL_CreateContext(display.window);
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    log_error("Failed creating GL context");
    return 0;
  }

  if (cvar_vsync.value.boolean) {
    if (SDL_GL_SetSwapInterval(1) == 0) {
      log_error("Failed setting vsync.\n");
    }
  } else {
    if (SDL_GL_SetSwapInterval(0) == 0) {
      log_error("Failed setting vsync.\n");
    }
  }

  SDL_GL_SetSwapInterval(0);

  // set viewport etc
  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glCullFace(GL_BACK);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_FRAMEBUFFER_SRGB);

  // lock mouse
  // SDL_SetRelativeMouseMode(true);
  // SDL_CaptureMouse(true);
  // SDL_SetWindowGrab(display.window, true);

  display.width = width;
  display.height = height;

  return 1;
}

void ex_window_event(SDL_Event *event) {
  switch (event->window.type) {
  case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
  case SDL_EVENT_WINDOW_RESIZED: {
    display.width = event->window.data1;
    display.height = event->window.data2;
    if (ex_resize_ptr) {
      ex_resize_ptr(event->window.data1, event->window.data2);
    }
    break;
  }
  default:
    break;
  }
}

void ex_window_destroy() {
  // bye bye
  SDL_GL_DestroyContext(display.context);
  SDL_DestroyWindow(display.window);
  SDL_Quit();
}
