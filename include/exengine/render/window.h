/* window
  The window module, handles
  opening a window.  As well
  as window events and input
  callbacks.
*/

#pragma once

#include <SDL3/SDL.h>
#include <inttypes.h>

typedef struct {
  SDL_Window *window;
  SDL_GLContext context;
  int width, height;
} ex_window_t;

extern ex_window_t display;

/**
 * @brief SDL window resize callback.
 * @param window the SDL window being resized
 * @param width  new width
 * @param height new height
 */
void ex_resize_callback(SDL_Window *window, int width, int height);

/**
 * @brief Create the window and GL context.
 * @param width  window width
 * @param height window height
 * @param title  window title
 * @return true on success
 */
int ex_window_init(uint32_t width, uint32_t height, const char *title);

/**
 * @brief Window event handler.
 * @param event SDL_Event pointer
 */
void ex_window_event(SDL_Event *event);

/**
 * @brief Clean up window data.
 */
void ex_window_destroy();
