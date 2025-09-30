#include <exengine/engine.h>
#include <exengine/input/input.h>
#include <exengine/render/camera.h>
#include <exengine/render/text.h>
#include <exengine/render/vga.h>
#include <game/game.h>
#include <game/mainmenu_scene.h>
#include <game/world_scene.h>

typedef enum current_scene_e {
  WORLD_SCENE,
  MAIN_MENU,
} current_scene_t;

current_scene_t current_scene = MAIN_MENU;
bool scene_changed = false;

void game_init() {
  mainmenu_scene_init();
}

void game_update(double dt, double ft) {
  if (scene_changed) {
    mainmenu_scene_exit();
    world_scene_init();
    current_scene = WORLD_SCENE;
    scene_changed = false;
  }

  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_update(dt, ft);
    break;
  case MAIN_MENU:
    mainmenu_scene_update(dt, ft);
    break;
  }
}

void game_draw() {
  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_draw();
    break;
  case MAIN_MENU:
    mainmenu_scene_draw();
    break;
  }
}

void game_exit() {
  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_exit();
    break;
  case MAIN_MENU:
    mainmenu_scene_exit();
    break;
  }
  ex_vga_destroy();
  printf("Good bye!\n");
}

void game_keypressed(uint32_t key) {
  if (current_scene != WORLD_SCENE) {
    current_scene = WORLD_SCENE;
    scene_changed = true;
  }

  // switch (current_scene) {
  // case WORLD_SCENE:
  //   world_scene_keypressed(key);
  //   break;
  // case MAIN_MENU:
  //   mainmenu_scene_keypressed(key);
  //   break;
  // }
}

void game_mousepressed(uint8_t button) {
  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_mousepressed(button);
    break;
  case MAIN_MENU:
    break;
  }
}

void game_mousemoition(int xrel, int yrel) {
  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_mousemoition(xrel, yrel);
    break;
  case MAIN_MENU:
    mainmenu_scene_mousemoition(xrel, yrel);
    break;
  }
}

void game_mousewheel(int32_t x, int32_t y) {
  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_mousewheel(x, y);
    break;
  case MAIN_MENU:
    mainmenu_scene_mousewheel(x, y);
    break;
  }
}

void game_resize(uint32_t width, uint32_t height) {
  switch (current_scene) {
  case WORLD_SCENE:
    world_scene_resize(width, height);
    break;
  case MAIN_MENU:
    mainmenu_scene_resize(width, height);
    break;
  }
}
