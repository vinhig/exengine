#include "log/log.h"

#include <exengine/render/vga.h>
#include <game/editor_scene.h>
#include <game/fps_scene.h>
#include <game/game.h>
#include <game/mainmenu_scene.h>
#include <game/world_scene.h>

typedef struct scene_vtable {
  void (*init)(void);
  void (*update)(double dt, double ft);
  void (*draw)(void);
  void (*exit)(void);
  void (*keypressed)(uint32_t key);
  void (*mousepressed)(uint8_t button);
  void (*mousemoition)(int xrel, int yrel);
  void (*mousewheel)(int32_t x, int32_t y);
  void (*resize)(uint32_t width, uint32_t height);
} scene_vtable_t;

static const scene_vtable_t scene_vtables[] = {
    [EDITOR_SCENE] = {
        .init = editor_scene_init,
        .update = editor_scene_update,
        .draw = editor_scene_draw,
        .exit = editor_scene_exit,
        .keypressed = editor_scene_keypressed,
        .mousepressed = editor_scene_mousepressed,
        .mousemoition = editor_scene_mousemoition,
        .mousewheel = editor_scene_mousewheel,
        .resize = editor_scene_resize,
    },
    [FPS_SCENE] = {
        .init = fps_scene_init,
        .update = fps_scene_update,
        .draw = fps_scene_draw,
        .exit = fps_scene_exit,
        .keypressed = fps_scene_keypressed,
        .mousepressed = fps_scene_mousepressed,
        .mousemoition = fps_scene_mousemoition,
        .mousewheel = fps_scene_mousewheel,
        .resize = fps_scene_resize,
    },
    [WORLD_SCENE] = {
        .init = world_scene_init,
        .update = world_scene_update,
        .draw = world_scene_draw,
        .exit = world_scene_exit,
        .keypressed = world_scene_keypressed,
        .mousepressed = world_scene_mousepressed,
        .mousemoition = world_scene_mousemoition,
        .mousewheel = world_scene_mousewheel,
        .resize = world_scene_resize,
    },
    [MAIN_MENU] = {
        .init = mainmenu_scene_init,
        .update = mainmenu_scene_update,
        .draw = mainmenu_scene_draw,
        .exit = mainmenu_scene_exit,
        .keypressed = mainmenu_scene_keypressed,
        .mousepressed = mainmenu_scene_mousepressed,
        .mousemoition = mainmenu_scene_mousemoition,
        .mousewheel = mainmenu_scene_mousewheel,
        .resize = mainmenu_scene_resize,
    },
};

current_scene_t current_scene = FPS_SCENE;

void game_change_scene(const current_scene_t new_scene) {
  scene_vtables[current_scene].exit();
  current_scene = new_scene;
  scene_vtables[current_scene].init();
}

void game_init() {
  scene_vtables[current_scene].init();
}

void game_update(double dt, double ft) {
  scene_vtables[current_scene].update(dt, ft);
}

void game_draw() {
  scene_vtables[current_scene].draw();
}

void game_exit() {
  scene_vtables[current_scene].exit();
  ex_vga_destroy();
  log_info("Good bye!");
}

void game_keypressed(uint32_t key) {
  scene_vtables[current_scene].keypressed(key);
}

void game_mousepressed(uint8_t button) {
  scene_vtables[current_scene].mousepressed(button);
}

void game_mousemoition(int xrel, int yrel) {
  scene_vtables[current_scene].mousemoition(xrel, yrel);
}

void game_mousewheel(int32_t x, int32_t y) {
  scene_vtables[current_scene].mousewheel(x, y);
}

void game_resize(uint32_t width, uint32_t height) {
  scene_vtables[current_scene].resize(width, height);
}
