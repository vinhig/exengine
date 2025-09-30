#pragma once

#include <stdint.h>

void world_scene_init();
void world_scene_update(double dt, double ft);
void world_scene_draw();
void world_scene_exit();
void world_scene_keypressed(uint32_t key);
void world_scene_mousepressed(uint8_t button);
void world_scene_mousemoition(int xrel, int yrel);
void world_scene_mousewheel(int32_t x, int32_t y);
void world_scene_resize(uint32_t width, uint32_t height);
