#pragma once

#include <stdint.h>

void fps_scene_init();
void fps_scene_update(double dt, double ft);
void fps_scene_draw();
void fps_scene_exit();
void fps_scene_keypressed(uint32_t key);
void fps_scene_mousepressed(uint8_t button);
void fps_scene_mousemoition(int xrel, int yrel);
void fps_scene_mousewheel(int32_t x, int32_t y);
void fps_scene_resize(uint32_t width, uint32_t height);
