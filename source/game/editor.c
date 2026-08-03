#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <glad/glad.h>
#include <imgui/cimgui.h>
#include <imgui/cimgui_impl.h>

#include <exengine/math/mathlib.h>
#include <exengine/render/camera.h>
#include <exengine/render/model.h>
#include <exengine/render/pointlight.h>
#include <exengine/render/renderer.h>
#include <exengine/render/scene.h>
#include <exengine/render/shader.h>
#include <exengine/render/window.h>
#include <exengine/util/iqm.h>
#include <game/editor_scene.h>

static ex_scene_t *editor_scene;
static ex_model_t *cube_model;
static ex_point_light_t *editor_light;
static GLuint viewport_fbo, viewport_color_tex, viewport_depth_rbo;
static int viewport_width, viewport_height;
static GLuint editor_shader;
static vec3 camera_pos = {3.0f, 2.0f, 3.0f};

static void editor_init_viewport_fbo(int width, int height) {
    if (viewport_fbo) {
        glDeleteTextures(1, &viewport_color_tex);
        glDeleteRenderbuffers(1, &viewport_depth_rbo);
        glDeleteFramebuffers(1, &viewport_fbo);
    }

    viewport_width = width;
    viewport_height = height;

    glGenFramebuffers(1, &viewport_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, viewport_fbo);

    glGenTextures(1, &viewport_color_tex);
    glBindTexture(GL_TEXTURE_2D, viewport_color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewport_color_tex, 0);

    glGenRenderbuffers(1, &viewport_depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, viewport_depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, viewport_depth_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void editor_render_viewport() {
    if (viewport_width <= 0 || viewport_height <= 0) return;

    if (!editor_shader) {
        editor_shader = ex_graphic_pipeline_new("forward");
        if (!editor_shader) return;
    }

    ex_camera_matrices_t camera_matrices;
    memset(&camera_matrices, 0, sizeof(camera_matrices));

    vec3 front = {-camera_pos[0], -camera_pos[1], -camera_pos[2]};
    vec3_norm(front, front);

    vec3 center;
    vec3_add(center, camera_pos, front);

    vec3 up = {0.0f, 1.0f, 0.0f};
    mat4x4_look_at(camera_matrices.view, camera_pos, center, up);
    mat4x4_invert(camera_matrices.inverse_view, camera_matrices.view);

    float aspect = (float)viewport_width / (float)viewport_height;
    float fov = 70.0f;
    mat4x4_perspective(camera_matrices.projection, rad(fov), aspect, 0.01f, 1000.0f);
    mat4x4_perspective(camera_matrices.projection_frustum, rad(fov) * 1.5f, aspect, 0.01f, 1000.0f);
    mat4x4_mul(camera_matrices.frustum, camera_matrices.projection_frustum, camera_matrices.view);

    glBindFramebuffer(GL_FRAMEBUFFER, viewport_fbo);
    glViewport(0, 0, viewport_width, viewport_height);
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    ex_graphic_pipeline_use(editor_shader);

    glUniformMatrix4fv(ex_uniform(editor_shader, "u_projection"), 1, GL_FALSE, camera_matrices.projection[0]);
    glUniformMatrix4fv(ex_uniform(editor_shader, "u_view"), 1, GL_FALSE, camera_matrices.view[0]);
    glUniformMatrix4fv(ex_uniform(editor_shader, "u_inverse_view"), 1, GL_FALSE, camera_matrices.inverse_view[0]);

    glUniform1i(ex_uniform(editor_shader, "u_point_active"), 1);
    glUniform1i(ex_uniform(editor_shader, "u_point_count"), 1);
    glUniform1i(ex_uniform(editor_shader, "u_ambient_pass"), 1);

    glUniform1i(ex_uniform(editor_shader, "u_point_lights[0].is_shadow"), 0);
    glUniform1f(ex_uniform(editor_shader, "u_point_lights[0].far"), EX_POINT_FAR_PLANE);
    glUniform3fv(ex_uniform(editor_shader, "u_point_lights[0].position"), 1, editor_light->position);
    glUniform3fv(ex_uniform(editor_shader, "u_point_lights[0].color"), 1, editor_light->color);

    ex_render_model(cube_model, &camera_matrices, editor_shader);

    glUniform1i(ex_uniform(editor_shader, "u_ambient_pass"), 0);
    glUniform1i(ex_uniform(editor_shader, "u_point_count"), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void editor_scene_init() {
    editor_scene = ex_scene_new(0);

    cube_model = ex_iqm_load_model(editor_scene, "data/cube.iqm", 0);
    cube_model->is_lit = 1;
    cube_model->cast_shadow = 0;
    cube_model->transform_fulls[0].scale = 1.0f;
    ex_scene_add_model(editor_scene, cube_model);

    editor_light = ex_point_light_new((vec3){2.0f, 5.0f, 2.0f}, (vec3){1.0f, 1.0f, 1.0f}, 0);
    editor_light->cast_shadow = 0;
    ex_scene_add_pointlight(editor_scene, editor_light);
}

void editor_scene_update(double dt, double ft) {
}

void editor_scene_draw() {
    bool opened = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;
    ImVec2 window_pos = {0, 0};
    ImVec2 window_size = {(float)display.width, (float)display.height};

    igSetNextWindowPos(window_pos, ImGuiCond_Always, (ImVec2){0, 0});
    igSetNextWindowSize(window_size, ImGuiCond_Always);

    igBegin("Scene Editor", &opened, window_flags);

    if (igBeginMenuBar()) {
        if (igBeginMenu("File", true)) {
            igMenuItem_Bool("New Scene", NULL, false, false);
            igMenuItem_Bool("Open Scene", NULL, false, false);
            igMenuItem_Bool("Save Scene", NULL, false, false);
            igEndMenu();
        }
        if (igBeginMenu("Edit", true)) {
            igMenuItem_Bool("Undo", NULL, false, false);
            igMenuItem_Bool("Redo", NULL, false, false);
            igEndMenu();
        }
        if (igBeginMenu("View", true)) {
            igMenuItem_Bool("Hierarchy", NULL, true, false);
            igMenuItem_Bool("Inspector", NULL, true, false);
            igEndMenu();
        }
        igEndMenuBar();
    }

    {
        float left_width = window_size.x * 0.2f;
        igBeginChild_Str("left_panel", (ImVec2){left_width, 0}, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX, ImGuiWindowFlags_None);
        igText("Scene Hierarchy");
        igSeparatorText("Objects");
        igText("  Camera");
        igText("  Directional Light");
        igText("  Cube");
        igEndChild();

        igSameLine(0.0f, 2.0f);

        ImVec2 avail;
        igGetContentRegionAvail(&avail);
        float remaining = avail.x;
        float inspector_width = window_size.x * 0.2f;
        float viewport_width = remaining - inspector_width - 2.0f;
        if (viewport_width < 100.0f) viewport_width = 100.0f;

        igBeginChild_Str("viewport", (ImVec2){viewport_width, 0}, ImGuiChildFlags_Borders, ImGuiWindowFlags_None);

        igGetContentRegionAvail(&avail);
        if (avail.x > 0 && avail.y > 0) {
            int vpw = (int)avail.x;
            int vph = (int)avail.y;

            if (vpw != viewport_width || vph != viewport_height) {
                editor_init_viewport_fbo(vpw, vph);
            }

            editor_render_viewport();

            ImTextureRef tex_ref;
            memset(&tex_ref, 0, sizeof(tex_ref));
            tex_ref._TexID = (ImU64)(uintptr_t)viewport_color_tex;
            igImage(tex_ref, (ImVec2){(float)viewport_width, (float)viewport_height},
                    (ImVec2){0, 1}, (ImVec2){1, 0});
        }

        igEndChild();

        igSameLine(0.0f, 2.0f);

        igBeginChild_Str("inspector", (ImVec2){0, 0}, ImGuiChildFlags_Borders, ImGuiWindowFlags_None);
        igText("Inspector");
        igSeparatorText("Transform");
        igText("  Position: 0.0, 0.0, 0.0");
        igText("  Rotation: 0.0, 0.0, 0.0");
        igText("  Scale:    1.0, 1.0, 1.0");
        igEndChild();
    }

    igEnd();
}

void editor_scene_exit() {
    if (viewport_fbo) {
        glDeleteTextures(1, &viewport_color_tex);
        glDeleteRenderbuffers(1, &viewport_depth_rbo);
        glDeleteFramebuffers(1, &viewport_fbo);
    }
}

void editor_scene_keypressed(uint32_t key) {
}

void editor_scene_mousepressed(uint8_t button) {
}

void editor_scene_mousemoition(int xrel, int yrel) {
}

void editor_scene_mousewheel(int32_t x, int32_t y) {
}

void editor_scene_resize(uint32_t width, uint32_t height) {
}
