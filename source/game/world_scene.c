#include "exengine/util/cvar.h"

#include <exengine/engine.h>
#include <exengine/input/input.h>
#include <exengine/render/camera.h>
#include <exengine/render/text.h>
#include <exengine/render/vga.h>
#include <exengine/util/entity.h>
#include <exengine/util/iqm.h>
#include <game/game.h>
#include <game/world_scene.h>

ex_fps_camera_t *camera;
ex_scene_t *scene;
ex_model_t *level, *d, *erebus_model;
ex_entity_t *camera_entity;
ex_point_light_t *l, *pl;
ex_source_t *sound;
ex_font_t *font;
float move_speed = 1.5f;

extern cvar_t cvar_screen_width;
extern cvar_t cvar_screen_height;

void world_scene_init() {
  // init the scene
  scene = ex_scene_new(0);
  memcpy(scene->gravity, (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));

  // init the camera
  camera = ex_fps_camera_new(5.0f, 5.0f, 0.0f, 0.1f, 70.0f);
  camera->yaw = 60.0f;
  camera->pitch -= 45.0f;

  level = ex_iqm_load_model(scene, "data/level_2.iqm", EX_KEEP_VERTICES);
  level->cast_shadow = 1;
  ex_scene_add_model(scene, level);

  camera_entity = ex_entity_new(scene, (vec3){0.5f, 1.0f, 0.5f});
  camera_entity->position[1] = 10.0f;
  camera_entity->position[0] = 5.0f;
  camera_entity->position[2] = 0.0f;

  // load a sound
  sound = ex_sound_load("data/ambient.ogg", EX_SOURCE_STREAMING, EX_SOURCE_LOOPING);
  ex_sound_master_volume(0.5f);

  pl = ex_point_light_new((vec3){0.0f, 5.0f, 4.0f}, (vec3){0.85f, 0.85f, 0.85f}, 1);
  ex_scene_add_pointlight(scene, pl);
  pl->cast_shadow = 1;

  pl = ex_point_light_new((vec3){0.0f, 5.0f, -4.0f}, (vec3){0.85f, 0.85f, 0.85f}, 1);
  ex_scene_add_pointlight(scene, pl);
  pl->cast_shadow = 1;

  erebus_model = ex_iqm_load_model(scene, "data/erebus.iqm", 0);
  erebus_model->cast_shadow = 1;
  erebus_model->scale = 0.025f;
  erebus_model->rotation[0] = -90.0f;
  erebus_model->position[0] = 5.0f;
  erebus_model->position[1] = 5.0f;
  erebus_model->position[2] = 5.0f;
  ex_scene_add_model(scene, erebus_model);

  ex_model_set_anim(erebus_model, "run");

  // this ain't it
  font = ex_font_load("data/fonts/OpenSans-Regular.ttf", "abcdefghijklmnopqrstuvwxyzHW!_");

  ex_vga_init();
}

void world_scene_update(double dt, double ft) {
  ex_entity_update(camera_entity, dt);

  // reduce velocity of the camera_entity
  camera_entity->velocity[0] *= (float)(1.0 - dt) * 0.95f;
  camera_entity->velocity[1] *= (float)(1.0 - dt) * 0.95f;
  camera_entity->velocity[2] *= (float)(1.0 - dt) * 0.95f;

  memcpy(camera->position, camera_entity->position, sizeof(vec3));
  camera->position[1] += camera_entity->radius[1];

  // memcpy(box->position, cube->position, sizeof(vec3));

  vec3 speed, side;
  if (ex_keys_down[SDL_SCANCODE_W]) {
    vec3_norm(speed, (vec3){camera->front[0], 0.0f, camera->front[2]});
    vec3_scale(speed, speed, move_speed * dt);
    speed[1] = 0.0f;
    vec3_add(camera_entity->velocity, camera_entity->velocity, speed);
  }
  if (ex_keys_down[SDL_SCANCODE_S]) {
    vec3_norm(speed, (vec3){camera->front[0], 0.0f, camera->front[2]});
    vec3_scale(speed, speed, move_speed * dt);
    speed[1] = 0.0f;
    vec3_sub(camera_entity->velocity, camera_entity->velocity, speed);
  }
  if (ex_keys_down[SDL_SCANCODE_A]) {
    vec3_mul_cross(side, camera->front, camera->up);
    vec3_norm(side, side);
    vec3_scale(side, side, (move_speed * 0.9f) * dt);
    side[1] = 0.0f;
    vec3_sub(camera_entity->velocity, camera_entity->velocity, side);
  }
  if (ex_keys_down[SDL_SCANCODE_D]) {
    vec3_mul_cross(side, camera->front, camera->up);
    vec3_norm(side, side);
    vec3_scale(side, side, (move_speed * 0.9f) * dt);
    side[1] = 0.0f;
    vec3_add(camera_entity->velocity, camera_entity->velocity, side);
  }

  ex_sound_play(sound);
  move_speed = 100.0f;

  ex_scene_update(scene, dt);
  ex_fps_camera_update(camera, false);

  ex_vga_clear();

  char buf[64];
  ex_vga_setfg(255, 255, 255, 255);
  ex_vga_setbg(0, 0, 0, 255);
  sprintf(buf, "exengine dbg build %c", 1);
  ex_vga_print(2, 1, buf);

  sprintf(buf, "framerate %.2f", 1.0 / ft);
  ex_vga_setfg(255, 255, 0, 255);
  ex_vga_setbg(255, 255, 255, 0);
  ex_vga_print(2, 2, buf);

  float ndc_x = (2.0f * ((float)ex_mouse_x + 0.5f)) / (float)cvar_screen_width.value.i32 - 1.0f;
  float ndc_y = 1.0f - (2.0f * ((float)ex_mouse_y + 0.5f)) / (float)cvar_screen_height.value.i32;

  vec4 near_point = {ndc_x, ndc_y, -1.0f, 1.0f};
  vec4 far_point = {ndc_x, ndc_y, 1.0f, 1.0f};

  mat4x4 view_proj;
  mat4x4_mul(view_proj, camera->matrices.projection, camera->matrices.view);
  mat4x4 inv_view_proj;
  mat4x4_invert(inv_view_proj, view_proj);

  float near_world[4], far_world[4];
  mat4x4_mul_vec4(near_world, inv_view_proj, near_point);
  mat4x4_mul_vec4(far_world, inv_view_proj, far_point);

  near_world[0] /= near_world[3];
  near_world[1] /= near_world[3];
  near_world[2] /= near_world[3];

  far_world[0] /= far_world[3];
  far_world[1] /= far_world[3];
  far_world[2] /= far_world[3];

  vec3 ray_direction;
  ray_direction[0] = far_world[0] - near_world[0];
  ray_direction[1] = far_world[1] - near_world[1];
  ray_direction[2] = far_world[2] - near_world[2];

  vec3_norm(ray_direction, ray_direction);

  ex_plane_t plane = {};
  vec3 infinite_vector;
  vec3_mul(infinite_vector, ray_direction, (vec3){9999.0f, 9999.0f, 9999.0f});
  // vec3_add(infinite_vector, camera->position, infinite_vector);
  float dist = ex_raycast(camera_entity, camera_entity->position, infinite_vector, &plane);

  if (dist > 0.0f) {
    erebus_model->position[0] = plane.intersection_point[0];
    erebus_model->position[1] = plane.intersection_point[1];
    erebus_model->position[2] = plane.intersection_point[2];
  }
}

void world_scene_draw() {
  ex_scene_draw(scene, 0, 0, 0, 0, &camera->matrices);
  ex_fps_camera_resize(camera);

  ex_font_dbg(font);

  ex_vga_render();
}

void world_scene_exit() {
  ex_scene_destroy(scene);
}

void world_scene_keypressed(uint32_t key) {
}

void world_scene_mousepressed(uint8_t button) {
}

void world_scene_mousemoition(int xrel, int yrel) {
}

void world_scene_mousewheel(int32_t x, int32_t y) {
}

void world_scene_resize(uint32_t width, uint32_t height) {
  ex_scene_resize(scene, width, height);
}
