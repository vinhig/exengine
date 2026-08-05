# EXTENSIVE AUDIT: exengine

## Project Overview

A C23 game engine (~5,000 lines of own code across 22 `.c` and 27 `.h` files) targeting a Skyrim-like sandbox. Currently renders instanced hex-tile worlds, skeletal animation, point-light shadows, MSDF text, SSAO, and 3D audio streaming. A foundation for an in-engine editor (ImGui) is also in progress.

---

## TIER 1 — HIGH (Runtime Crashes / Resource Corruption)

### 1. Null Function Pointers Called Without Checks

`source/engine.c:130,138,212,226,245`: All user callback pointers (`ex_init_ptr`, `ex_update_ptr`, `ex_draw_ptr`, `ex_exit_ptr`) are called with zero NULL guard. If any is unset → **immediate crash**.

### 2. Multiple NULL Dereferences in I/O Layer

- `source/util/io.c:19-22`: `PHYSFS_openRead` result not checked for NULL before `PHYSFS_fileLength`.
- `source/util/io.c:28-31`: `calloc` result not checked before `PHYSFS_readBytes`.
- `source/render/texture.c:25-26`: Same `PHYSFS_openRead` issue.
- `source/render/texture.c:29`: Same `calloc` issue.

### 3. Buffer Overflow via `sprintf` in Engine Init

`source/engine.c:87`: `sprintf(data_path, "%s/data", write_path)` with 2048-byte `data_path`. If `write_path` exceeds ~2043 chars → **stack buffer overflow**. Use `snprintf`.

### 4. GL Texture Leak on Window Resize

`source/render/renderer.c:474,484`: Every `ex_render_resize()` call generates **new** textures via `glGenTextures` instead of re-specifying storage on existing textures → **GL object leak** on every resize.

### 5. Sound Buffer Delete Logic Reversed

`source/sound/sound.c:303-309`: Streaming sources (3 buffers created at line 170) have only 1 deleted; static sources (1 buffer created at line 198) have 3 deleted → **streaming leaks 2 buffers per source, static triggers invalid GL delete**.

### 6. Memory Leak in Texture Load Error Path

`source/render/texture.c:37-40`: If `stbi_load_from_memory` fails, `buff` (allocated at line 29) is never freed.

### 7. Memory Leak: Streaming Audio File Data

`source/sound/sound.c`: For streaming sources, `file_data` is passed to `stb_vorbis_open_memory` (line 134) which takes ownership, but no mechanism exists to free it when the source is destroyed.

### 8. Missing GL Link Status Check

`source/render/shader.c:128`: `glLinkProgram` is called but `glGetProgramiv(program, GL_LINK_STATUS, ...)` is never checked. A failed link produces a silently broken shader.

### 9. `SDL_GL_MakeCurrent` Called Before Context Creation

`source/render/window.c:46-47`: `SDL_GL_MakeCurrent(display.window, display.context)` is called **before** `SDL_GL_CreateContext`, with `display.context` uninitialized (NULL).

### 10. VSync Logic Broken

`source/render/window.c:53-63`: Error check is inverted (logs error on success), then line 63 unconditionally overrides with `SDL_GL_SetSwapInterval(0)`.

### 11. Wrong Callback Guard in Input

`source/input/input.c:29`: Checks `ex_keypressed_ptr != NULL` before calling `ex_mousepressed_ptr` (wrong callback). Mouse events won't fire if only the mouse callback is set.

### 12. IQM Out-of-Bounds Read Risk

`source/util/iqm.c:41`: Reads `sizeof(ex_iqm_header_t)` bytes from data without verifying the file is that large. Earlier check only validates the magic (16 bytes) and version.

---

## TIER 2 — MEDIUM (Design / Architecture)

### 13. VGA Debug Overlay Wastes 33MB

`source/render/vga.c:77-82`: `ex_vga_data` allocates `8 * 16 * 256 * 256 * sizeof(uint32_t)` = **~33.5 MB** but only uploads a 256×256 texture (~256 KB). 99% of the buffer is wasted.

### 14. CMake Uses `GLOB_RECURSE` for Source Files

`CMakeLists.txt:4`: Uses `GLOB_RECURSE ... CONFIGURE_DEPENDS`. The `CONFIGURE_DEPENDS` flag makes it re-glob on build, but best practice is still to list source files explicitly.

### 15. Address Sanitizer in All Builds

`CMakeLists.txt:80-83`: `-fsanitize=address,undefined` is always enabled on Linux (even Release), degrading performance ~2×. Should be Debug-only.

### 16. Uniform Lookups Every Frame (Performance)

`source/render/renderer.c:190-192,307-322`: Uniform location queries (`ex_uniform()`) are called every frame per-model. These should be cached once after shader compilation.

### 17. `glBufferData` Every Frame for Instances

`source/render/renderer.c:317`: Reallocates GPU instance buffer every frame instead of using `glBufferSubData`.

### 18. Frustum Culling Commented Out

`source/render/renderer.c:299-301`: All model instances are always rendered. Culling code exists but is disabled.

### 19. SSAO Resize Not Handled

`source/render/ssao.c`: SSAO textures are sized at init and never resized when the window changes.

### 20. SSAO Blur Pass Never Executed

`source/render/ssao.c`: `ssao_blur_fbo`, `ssao_color_blur_buffer`, and `ssao_blur_shader` are created (lines 99-118) but the blur pass is never called. Dead code.

### 21. CVAR System Partially Implemented

- `data_ptr` field — declared in `cvar.h:43`, never used.
- `callback` function pointer — declared in `cvar.h:45`, never invoked.
- `CVAR_SSAO_ENABLED` enum ID exists in `cvar.h` but no cvar instance is registered.
- (STR/BOOL defaults are now handled and persisted, unlike before.)

### 22. `ex_ini_load_dont_use()` is the Only File-Based INI Loader

`include/exengine/util/ini.h:68` and `source/util/cvar.c:14`: The sole file-loading function still has `dont_use` in its name and `@deprecated`. `ex_ini_parse()` now exists as the recommended in-memory API, but no non-deprecated file loader replaces it.

### 23. Model Copy Uses Shallow References

`source/render/model.c:48-58`: `ex_model_copy` deep-copies meshes and collision vertices and marks copies `is_copy` to avoid double-freeing (model.c:71,203-204), but animation/bone/frame/skeleton data is still shared with the original. If the original is freed first, all copies have dangling pointers.

### 24. Fragile Teardown Order Between Cache and Scene

`source/util/cache.c:34` now returns copies of cached models, so `ex_scene_destroy` (scene.c:131) operates on copies rather than cached originals. However, `engine.c:237` flushes the cache (destroying originals) **before** the user exit callback at `engine.c:245` destroys the scene copies, leaving copies holding freed shared animation data.

### 25. Fixed-Size Resource Limits Without Checks

- `EX_MODEL_MAX_MESHES 128` (model.h:17) — live.
- `MAX_GLYPH 512` (text.h:12) — live.
- `EX_MAX_POINT_LIGHTS 500` (scene.h:23) — now unused.
- `EX_MAX_SPOT_LIGHTS 128` (scene.h:24) — dead code.
- `EX_MAX_REFLECTIONS 50` (scene.h:25) — dead code.
- INI: 256 sections, 256 vars/section, 256 chars/key (ini.h).

### 26. Hardcoded Paths

- `EX_SHADER_LOC "data/shaders/"` (shader.h:11)
- `EX_TEXTURE_LOC "data/textures/"` (texture.h:11)
- `EX_DATA_FILE "data.ex"` (engine.h:19)
- Texture, shader, and IQM paths all concatenate these path constants; none come from CMake.

### 27. Uniform Cache is a File-Scope Mutable Global

`source/render/shader.c`: Static file-scope globals for the uniform cache. No thread safety, no way to reset/clear. If shader slots run out or arrays collide, stale values leak.

---

## TIER 3 — LOW (Minor / Cosmetic)

### 28. Typo `mousemoition` in the Game Layer

The engine layer now uses the correct spelling (`ex_mousemotion_ptr` in `engine.h`, `engine.c:29`, `input.c:43`, `main.c:12`), but the typo persists throughout the game layer: `game.c:17,103`, and all four scenes (`fps_scene.c:229`, `world_scene.c:280`, `mainmenu_scene.c:21`, `editor.c:228`) plus their headers (`game.h`, `fps_scene.h`, `world_scene.h`, `mainmenu_scene.h`, `editor_scene.h`).

### 29. Typo `obj_lenghts` in Octree

`source/math/octree.c:92`: Should be `obj_lengths`.

### 30. Typo `EX_SCENE_BIGGEST_LIGHT`

`include/exengine/render/scene.h:28`: Typo and never used anywhere.

### 31. Typo `mat4x4o_mul_quat`

`include/exengine/math/mathlib.h:736`: Should be `mat4x4_mul_quat`. Unused.

### 32. Camera FOV Documentation Wrong

`include/exengine/render/camera.h:29`: Header says FOV is in radians, but `camera.c:52` calls `rad(fov)` (converts degrees to radians). Actual FOV is in degrees.

### 33. `#include <float.h>` Mid-File in mathlib.h

Line 635 — should be at the top.

### 34. `ColumnLimit: 0` in .clang-format

No line length limit; can produce very long lines.

### 35. `Language: Cpp` in .clang-format for C Files

May misformat C-specific idioms.

### 36. Empty `licence/` Directory

Exists alongside `LICENSE` file — appears to be a typo that was never cleaned up.

### 37. `sprintf` with Non-Printable Character

`source/game/fps_scene.c:188` (and `world_scene.c:203`): `sprintf(buf, "exengine dbg build %c", 1)` prints ASCII SOH (non-printable).

### 38. Unused `font` Variable in FPS Scene

`source/game/fps_scene.c:56`: Font loaded but never used beyond `ex_font_dbg()`.

### 39. `ex_font_dbg` Renders Typo `"Heljo World!"`

`source/render/text.c:134`: Debug text has a typo.

### 40. World Scene Computes Ray Direction Every Frame Unused

`source/game/world_scene.c:211-245`: Full NDC→world raycast math computed every frame but the raycasting code is commented out.

### 41. Mixed `float`/`double` in Collision

`include/exengine/math/collision.h:35-36`: `ex_coll_packet_t` has `double t` but `float nearest_distance`. Mixed precision.

---

## PRIORITY RANKING

| # | Issue | Effort | Impact |
|---|-------|--------|--------|
| 1 | Add NULL guards to all callback invocations | Small | Crash |
| 2 | Add NULL guards to I/O and texture loading | Small | Crash |
| 3 | Fix `SDL_GL_MakeCurrent` order | Tiny | Crash |
| 4 | Fix VSync logic | Tiny | Logic |
| 5 | Fix sound buffer delete swap | Tiny | Crash |
| 6 | Add GL link status check | Small | Silent failure |
| 7 | Fix GL texture leak in resize | Small | Leak |
| 8 | Fix streaming audio file_data leak | Small | Leak |
| 9 | Address sanitizer only in Debug | Small | Perf |
| 10 | Cache uniform lookups | Medium | Performance |
| 11 | Enable frustum culling | Medium | Performance |
| 12 | Fix shallow model copy / teardown order | Medium | Correctness |
| 13 | Fix `sprintf` → `snprintf` in engine.c | Small | Security |
| 14 | Replace `GLOB_RECURSE` with explicit source list | Small | Build hygiene |
