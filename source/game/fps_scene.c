#include <exengine/engine.h>
#include <exengine/input/input.h>
#include <exengine/render/camera.h>
#include <exengine/render/text.h>
#include <exengine/render/vga.h>
#include <exengine/util/entity.h>
#include <exengine/util/iqm.h>
#include <game/fps_scene.h>
#include <game/game.h>

static ex_fps_camera_t *camera;
static ex_scene_t *scene;
static ex_model_t *m6, *box;
static ex_entity_t *cube, *e;
static ex_point_light_t *pl;
static ex_source_t *sound;
static ex_font_t *font;
static float move_speed = 1.5f;

void fps_scene_init()
{
  scene = ex_scene_new(0);
  memcpy(scene->gravity, (vec3){0.0f, -0.1f, 0.0f}, sizeof(vec3));

  camera = ex_fps_camera_new(0.0f, 0.0f, 0.0f, 0.1f, 70.0f);

  m6 = ex_iqm_load_model(scene, "data/level.iqm", EX_KEEP_VERTICES);
  m6->cast_shadow = 1;
  m6->transform_fulls[0].scale = 1.0f;
  ex_scene_add_model(scene, m6);

  e = ex_entity_new(scene, (vec3){0.5f, 1.0f, 0.5f});
  e->position[1] = 10.0f;
  e->position[0] = 5.0f;
  e->position[2] = 0.0f;

  sound = ex_sound_load("data/ambient.ogg", EX_SOURCE_STREAMING, EX_SOURCE_LOOPING);
  ex_sound_master_volume(0.5f);

  pl = ex_point_light_new((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.1f, 0.1f, 0.1f}, 0);
  memcpy(pl->position, e->position, sizeof(vec3));
  ex_scene_add_pointlight(scene, pl);
  pl->cast_shadow = 0;

  box = ex_iqm_load_model(scene, "data/cube.iqm", 0);

  box->is_lit = 0;
  box->cast_shadow = 0;
  box->transform_fulls[0].scale = 1.0f;
  ex_scene_add_model(scene, box);
  cube = ex_entity_new(scene, (vec3){0.95f, 0.95f, 0.95f});
  cube->position[2] = 5.0f;
  cube->position[1] = 5.0f;
  cube->position[0] = 0.0f;

  font = ex_font_load("data/fonts/OpenSans-Regular.ttf", "abcdefghijklmnopqrstuvwxyzHW!_");

  ex_vga_init();
}

void fps_scene_update(double dt, double ft)
{
  ex_entity_update(e, dt);
  ex_entity_update(cube, dt);

  memcpy(camera->position, e->position, sizeof(vec3));
  camera->position[1] += e->radius[1];
  memcpy(pl->position, camera->position, sizeof(vec3));

  memcpy(box->transform_fulls[0].position, cube->position, sizeof(vec3));

  vec3 temp;
  vec3_sub(temp, cube->position, e->position);
  float len = vec3_len(temp);
  if (len <= cube->radius[1]/2 + e->radius[1]) {
    vec3_norm(temp, temp);
    vec3_scale(temp, temp, vec3_len(e->velocity));
    vec3_add(cube->velocity, cube->velocity, temp);
    vec3_sub(e->velocity, e->velocity, temp);
  }

  vec3_scale(temp, cube->velocity, 5.0f * dt);
  temp[1] = 0.0f;
  if (cube->grounded == 1)
    vec3_sub(cube->velocity, cube->velocity, temp);

  cube->velocity[1] -= (100.0f * dt);

  if (ex_keys_down[SDL_SCANCODE_LCTRL]) {
    vec3 p;
    vec3_scale(p, camera->front, 2.5f);
    vec3_add(p, p, e->position);
    p[1] += 0.5f;

    vec3_sub(p, p, cube->position);

    float f = vec3_len(p);
    if (f > 1.5f) {
      ex_keys_down[SDL_SCANCODE_LCTRL] = 0;
      goto ctrl_end;
    }

    if (f > 1.0f)
      f *= f;

    vec3_norm(p, p);
    vec3_scale(p, p, f*35.0f);
    f = cube->velocity[1];
    memcpy(cube->velocity, p, sizeof(vec3));
    cube->velocity[1] += f*0.1f;

    if (ex_buttons_down[SDL_BUTTON_RIGHT]) {
      vec3_scale(temp, camera->front, 80.0f);
      vec3_add(cube->velocity, cube->velocity, temp);
      ex_keys_down[SDL_SCANCODE_LCTRL] = 0;
    }
  }
ctrl_end:

  if (ex_keys_down[SDL_SCANCODE_F]) {
    float r = (float)rand()/(float)(RAND_MAX/1.0f);
    float g = (float)rand()/(float)(RAND_MAX/1.0f);
    float b = (float)rand()/(float)(RAND_MAX/1.0f);
    ex_point_light_t *l = ex_point_light_new((vec3){0.0f, 0.0f, 0.0f}, (vec3){r, g, b}, 0);
    memcpy(l->position, camera->position, sizeof(vec3));
    ex_scene_add_pointlight(scene, l);
    l->cast_shadow = 1;
    ex_keys_down[SDL_SCANCODE_F] = 0;
  }

  vec3_scale(temp, e->velocity, 15.0f * dt);
  temp[1] = 0.0f;

  if (e->grounded == 1)
   vec3_sub(e->velocity, e->velocity, temp);
  else
    move_speed = 50.0f;

  e->velocity[1] -= (100.0f * dt);

  vec3 speed, side;
  if (ex_keys_down[SDL_SCANCODE_W]) {
    vec3_norm(speed, (vec3){camera->front[0], 0.0f, camera->front[2]});
    vec3_scale(speed, speed, move_speed * dt);
    speed[1] = 0.0f;
    vec3_add(e->velocity, e->velocity, speed);
  }
  if (ex_keys_down[SDL_SCANCODE_S]) {
    vec3_norm(speed, (vec3){camera->front[0], 0.0f, camera->front[2]});
    vec3_scale(speed, speed, move_speed * dt);
    speed[1] = 0.0f;
    vec3_sub(e->velocity, e->velocity, speed);
  }
  if (ex_keys_down[SDL_SCANCODE_A]) {
    vec3_mul_cross(side, camera->front, camera->up);
    vec3_norm(side, side);
    vec3_scale(side, side, (move_speed*0.9f) * dt);
    side[1] = 0.0f;
    vec3_sub(e->velocity, e->velocity, side);
  }
  if (ex_keys_down[SDL_SCANCODE_D]) {
    vec3_mul_cross(side, camera->front, camera->up);
    vec3_norm(side, side);
    vec3_scale(side, side, (move_speed*0.9f) * dt);
    side[1] = 0.0f;
    vec3_add(e->velocity, e->velocity, side);
  }
  if (ex_keys_down[SDL_SCANCODE_Q])
    e->velocity[1] = 50.0f;
  if (ex_keys_down[SDL_SCANCODE_Z])
    e->velocity[1] = -50.0f;
  if (ex_keys_down[SDL_SCANCODE_SPACE] && e->grounded == 1) {
    e->velocity[1] = 20.0f;
  }
  ex_sound_play(sound);
  move_speed = 100.0f;

  memcpy(pl->position, e->position, sizeof(vec3));
  pl->position[1] += 1.0f;
  ex_scene_update(scene, dt);
  ex_fps_camera_update(camera, true);

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
}

void fps_scene_draw()
{
  ex_scene_draw(scene, &camera->matrices);
  ex_fps_camera_resize(camera);

  ex_font_dbg(font);

  ex_vga_render();
}

void fps_scene_exit()
{
  ex_sound_destroy(sound);
  ex_fps_camera_destroy(camera);
  ex_font_destroy(font);
  free(e);
  free(cube);

  ex_scene_destroy(scene);
}

void fps_scene_keypressed(uint32_t key)
{
  if (key == SDL_SCANCODE_ESCAPE)
    game_change_scene(WORLD_SCENE);
}

void fps_scene_mousepressed(uint8_t button)
{
  ex_sound_restart(sound);
}

void fps_scene_mousemoition(int xrel, int yrel)
{
}

void fps_scene_mousewheel(int32_t x, int32_t y)
{
}

void fps_scene_resize(uint32_t width, uint32_t height)
{
  ex_scene_resize(scene, width, height);
}
