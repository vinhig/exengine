#pragma once

#include <stdint.h>

void editor_scene_init();
void editor_scene_update(double dt, double ft);
void editor_scene_draw();
void editor_scene_exit();
void editor_scene_keypressed(uint32_t key);
void editor_scene_mousepressed(uint8_t button);
void editor_scene_mousemoition(int xrel, int yrel);
void editor_scene_mousewheel(int32_t x, int32_t y);
void editor_scene_resize(uint32_t width, uint32_t height);
