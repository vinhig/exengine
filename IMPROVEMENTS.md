# EXTENSIVE AUDIT: exengine

## Project Overview

A C23 game engine (~5,000 lines of own code across 22 `.c` and 27 `.h` files) targeting a Skyrim-like sandbox. Currently renders instanced hex-tile worlds, skeletal animation, point-light shadows, MSDF text, SSAO, and 3D audio streaming.

---

## TIER 0 — CRITICAL (Won't Compile or Immediate Crashes)

### 1. `auto` Keyword Used as C++, Not C23 (9 instances)

Files: `shader.c:85,87-89`, `cvar.c:26,37,48,59`, `ini.c:228`

In C, `auto` means "automatic storage duration" (the default). Type inference via `auto` is **C++ only**.

- `source/render/shader.c:85`: `auto program = glCreateProgram();` — will fail with any C compiler.
- `source/util/cvar.c:26`: `auto value = ex_ini_get_float(...)` — same issue.

### 2. `constexpr` is C++, Not C

`source/render/ssao.c:14`: `constexpr size_t kernelSize = 16;` — `constexpr` does not exist in any C standard. This fails to compile as C.

### 3. `nullptr` Requires C23 Compiler (88 uses across `.c` files)

`nullptr` was added in C23. GCC 14+ or Clang 18+ required. Most CI/distro compilers don't support this yet. If the project truly targets C23, it must also use `{}` empty initializers (C23), which it does (e.g., `engine.c:227`).

**Assessment: This project is compiled as C++ (or with a C++ compiler in C mode), not as C23.** The CMakeLists.txt claims `CMAKE_C_STANDARD 23` but the code uses C++-only keywords.

### 4. Missing Compile Definitions in CMakeLists.txt

`source/engine.c:101`, `source/render/texture.c:11`, `source/render/renderer.c:58` reference these undefined macros:
- `EX_DATA_FILE` — **not defined anywhere** → compilation error
- `EX_TEXTURE_LOC` — **not defined anywhere** → compilation error
- `SHADOW_MAP_SIZE` — **not defined anywhere** → compilation error
- `USING_IMGUI` — defined nowhere → all ImGui code is dead

### 5. `main.c` Linked Twice

`CMakeLists.txt:68` adds `source/main.c` explicitly, but `file(GLOB_RECURSE)` on line 4 also collects it → **duplicate symbol / linker error**.

### 6. Buffer Overflow: Uniform Cache Indexed by Raw OpenGL ID

`include/exengine/render/shader.h` and `source/render/shader.c`: Global array `ex_uniform_map[256][256]` uses shader ID as the first index. OpenGL shader IDs are arbitrary integers (e.g., 1, 2, 3…). Any shader ID >= 256 causes **out-of-bounds write**. This is a **critical security vulnerability**.

### 7. Non-Standard Type `uint` in IQM Loader

`source/util/iqm.c:31`: `uint *head = (uint *)&data[16]` — `uint` is a GLib/Windows typedef, not standard C. Fails on platforms without it.

### 8. Logical `||` Instead of Bitwise `|` in IQM Loader

`source/util/iqm.c:151`: `anims[i].loop = a->flags || (1 << 0)` — uses logical OR `||` instead of bitwise `|`. Since `(1 << 0)` is always 1 (truthy), **every animation is forced to loop**. Should be `a->flags | (1 << 0)`.

---

## TIER 1 — HIGH (Runtime Crashes / Resource Corruption)

### 9. Null Function Pointers Called Without Checks

`source/engine.c:129,137,211,232,255`: All user callback pointers (`ex_init_ptr`, `ex_update_ptr`, `ex_draw_ptr`, `ex_exit_ptr`, `ex_resize_ptr`) are called with zero NULL guard. If any is unset → **immediate crash**.

### 10. Multiple NULL Dereferences in I/O Layer

- `source/util/io.c:18-19`: `PHYSFS_openRead` result not checked for NULL before `PHYSFS_fileLength`.
- `source/util/io.c:29`: `calloc` result not checked before `PHYSFS_readBytes`.
- `source/render/texture.c:24-26`: Same `PHYSFS_openRead` issue.
- `source/render/texture.c:29`: Same `calloc` issue.

### 11. Buffer Overflow via `sprintf` in Engine Init

`source/engine.c:86`: `sprintf(data_path, "%s/data", write_path)` with 2048-byte `data_path`. If `write_path` exceeds ~2043 chars → **stack buffer overflow**. Use `snprintf`.

### 12. Buffer Overflow via `sprintf` in Renderer

`source/render/renderer.c:199,205`: `sprintf(buff, "u_point_lights[%u]", pcount++)` with 64-byte buffer. If light count exceeds ~10 digits, overflows. Also `%u` is wrong for `size_t` (UB). Use `%zu` and `snprintf`.

### 13. GL Index Type Mismatch in Instanced Rendering

`source/render/renderer.c:366`: `glDrawElementsInstanced` hardcodes `GL_UNSIGNED_INT`. If a mesh uses 16-bit indices, this reads garbage → **GPU crash/rendering corruption**.

### 14. GL Texture Leak on Window Resize

`source/render/renderer.c:474,484`: Every `ex_render_resize()` call generates **new** textures via `glGenTextures` instead of re-specifying storage on existing textures → **GL object leak** on every resize.

### 15. Sound Buffer Delete Logic Reversed

`source/sound/sound.c:303-309`: Streaming sources (3 buffers) have only 1 deleted; static sources (1 buffer) have 3 deleted → **streaming leaks 2 buffers per source, static triggers invalid GL delete**.

### 16. Memory Leak in Texture Load Error Path

`source/render/texture.c:37-40`: If `stbi_load_from_memory` fails, `buff` (allocated at line 29) is never freed.

### 17. Memory Leak: Streaming Audio File Data

`source/sound/sound.c`: For streaming sources, `file_data` is passed to `stb_vorbis_open_memory` which takes ownership, but no mechanism exists to free it when the source is destroyed.

### 18. Missing GL Link Status Check

`source/render/shader.c`: `glLinkProgram` is called but `glGetProgramiv(program, GL_LINK_STATUS, ...)` is never checked. A failed link produces a silently broken shader.

### 19. `SDL_GL_MakeCurrent` Called Before Context Creation

`source/render/window.c:46-47`: `SDL_GL_MakeCurrent(display.window, display.context)` is called **before** `SDL_GL_CreateContext`, with `display.context` uninitialized (NULL).

### 20. VSync Logic Broken

`source/render/window.c:53-63`: Error check is inverted (logs error on success), then line 63 unconditionally overrides with `SDL_GL_SetSwapInterval(0)`.

### 21. Wrong Callback Guard in Input

`source/input/input.c:29`: Checks `ex_keypressed_ptr != NULL` before calling `ex_mousepressed_ptr` (wrong callback). Mouse events won't fire if only the mouse callback is set.

### 22. IQM Out-of-Bounds Read Risk

`source/util/iqm.c:41`: Reads `sizeof(ex_iqm_header_t)` bytes from data without verifying the file is that large. Earlier check only validates 20 bytes.

---

## TIER 2 — MEDIUM (Design / Architecture)

### 23. RHI Layer is Dead Code

`source/render/rhi.c` (789 lines) defines an OpenGL abstraction, but **none** of the rendering code (`renderer.c`, `mesh.c`, `ssao.c`, `text.c`, `vga.c`, `pointlight.c`, `camera.c`) uses it — they all call raw OpenGL directly. Either integrate the RHI or remove it.

### 24. Shader Compilation Duplicated

`source/render/shader.c:ex_graphic_pipeline_new()` and `source/render/rhi.c:ex_gpu_pipeline_create()` are two independent implementations of the same shader compilation logic.

### 25. VGA Debug Overlay Wastes 33MB

`source/render/vga.c`: `ex_vga_data` allocates `8 * 16 * 256 * 256 * sizeof(uint32_t)` = **~33.5 MB** but only uploads a 256×256 texture (~256 KB). 99% of the buffer is wasted.

### 26. CMake Uses `GLOB_RECURSE` for Source Files

`CMakeLists.txt:4`: CMake best practice is to list source files explicitly. `GLOB_RECURSE` doesn't auto-detect new/deleted files without re-running CMake.

### 27. Address Sanitizer in All Builds

`CMakeLists.txt:78-81`: `-fsanitize=address,undefined` is always enabled (even Release), degrading performance ~2×. Should be Debug-only.

### 28. Uniform Lookups Every Frame (Performance)

`source/render/renderer.c:307,312,320,330`: Uniform location queries (`ex_uniform()`) are called every frame per-model. These should be cached once after shader compilation.

### 29. `glBufferData` Every Frame for Instances

`source/render/renderer.c:317`: Reallocates GPU instance buffer every frame instead of using `glBufferSubData`.

### 30. Frustum Culling Commented Out

`source/render/renderer.c:299-301`: All model instances are always rendered. Culling code exists but is disabled.

### 31. SSAO Resize Not Handled

`source/render/ssao.c`: SSAO textures are sized at init and never resized when the window changes.

### 32. SSAO Blur Pass Never Executed

`source/render/ssao.c`: `ssao_blur_fbo` and `ssao_blur_shader` are created but the blur pass is never called. Dead code.

### 33. CVAR System Partially Implemented

- `data_ptr` field — declared, never used.
- `callback` function pointer — declared, never invoked.
- `CVAR_STR` and `CVAR_BOOL` defaults — never persisted to INI (unlike F32/I32).
- `CVAR_SSAO_ENABLED` enum ID exists in `cvar.h` but no cvar instance is registered.

### 34. `ex_ini_load_dont_use()` is the Only INI Loader

`include/exengine/util/ini.h`: The sole function that loads INI files has `dont_use` in its name and `@deprecated`. No replacement exists.

### 35. Model Copy Uses Shallow References

`source/render/model.c`: `ex_model_copy` points to the same animation/bone data as the original. If the original is freed first, all copies have dangling pointers.

### 36. `ex_scene_destroy` Frees Cached Models

`source/render/scene.c`: Destroys models that may be shared via the cache. Use-after-free if cache later accesses them.

### 37. Fixed-Size Resource Limits Without Checks

- `EX_MODEL_MAX_MESHES 128` (model.h)
- `MAX_GLYPH 512` (text.h)
- `EX_MAX_POINT_LIGHTS 500` (scene.h)
- `EX_MAX_SPOT_LIGHTS 128` (dead code)
- `EX_MAX_REFLECTIONS 50` (dead code)
- INI: 256 sections, 256 vars/section, 256 chars/key

### 38. Hardcoded Paths

- `EX_SHADER_LOC "data/shaders/"` — defined in both `shader.h` and `rhi.c`
- `EX_TEXTURE_LOC` — should come from CMake
- Texture, shader, and IQM paths all concatenate different path constants

### 39. `ex_uniform_map` is a File-Scope Mutable Global

No thread safety, no way to reset/clear. If shader IDs wrap or arrays collide, stale values leak.

---

## TIER 3 — LOW (Minor / Cosmetic)

### 40. Typo `mousemoition` Propagated Across Entire Codebase

`mousemoition` instead of `mousemotion` in: `engine.h:18`, all game headers (`game.h`, `fps_scene.h`, `world_scene.h`, `mainmenu_scene.h`), `game.c`, and `engine.c`.

### 41. Typo `obj_lenghts` in Octree

`source/math/octree.c:92`: Should be `obj_lengths`.

### 42. Typo `EX_SCENE_BIGGEST_LIGHT`

`include/exengine/render/scene.h`: Typo and never used anywhere.

### 43. Typo `mat4x4o_mul_quat`

`include/exengine/math/mathlib.h`: Should be `mat4x4_mul_quat`.

### 44. Camera FOV Documentation Wrong

Header says FOV is in radians, but `camera.c` calls `rad(fov)` (converts degrees to radians). Actual FOV is in degrees.

### 45. `#include <float.h>` Mid-File in mathlib.h

Line 617 — should be at the top.

### 46. `ColumnLimit: 0` in .clang-format

No line length limit; can produce very long lines.

### 47. `Language: Cpp` in .clang-format for C Files

May misformat C-specific idioms.

### 48. Empty `licence/` Directory

Exists alongside `LICENSE` file — appears to be a typo that was never cleaned up.

### 49. `sprintf` with Non-Printable Character

`source/game/fps_scene.c:175`: `sprintf(buf, "exengine dbg build %c", 1)` prints ASCII SOH (non-printable).

### 50. Unused `font` Variable in FPS Scene

Font loaded but never used beyond `ex_font_dbg()`.

### 51. `ex_font_dbg` Renders Typo `"Heljo World!"`

Debug text has a typo.

### 52. World Scene Computes Ray Direction Every Frame Unused

`source/game/world_scene.c:211-239`: Full NDC→world raycast math computed every frame but the raycasting code is commented out.

### 53. `glPolygonMode` Leak in Octree Rendering

`source/math/octree.c`: Debug rendering uses `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` without restoring state.

_Resolved: the octree debug renderer was removed — the octree is now collision-only._

### 54. `fabs` vs `fabsf` in Collision

`source/math/collision.c:171`: `fabs()` (double) used with float values. Use `fabsf()`.

### 55. Mixed `float`/`double` in Collision

`ex_coll_packet_t` has `double t` but `float nearest_distance`. Mixed precision.

### 56. No Tests

No test framework, no test files, no CI test step.

---

## PRIORITY RANKING

| # | Issue | Effort | Impact |
|---|-------|--------|--------|
| 1 | Decide C vs C++ target (`auto`/`constexpr`/`nullptr` fiasco) | Medium | Build |
| 2 | Add missing CMake definitions | Small | Build |
| 3 | Fix `main.c` double-link | Small | Build |
| 4 | Fix uniform cache buffer overflow | Small | Crash |
| 5 | Fix `||` vs `|` in IQM loader | Tiny | Logic |
| 6 | Fix `uint` in IQM loader | Tiny | Build |
| 7 | Add NULL guards to all callback invocations | Small | Crash |
| 8 | Add NULL guards to I/O and texture loading | Small | Crash |
| 9 | Fix `SDL_GL_MakeCurrent` order | Tiny | Crash |
| 10 | Fix VSync logic | Tiny | Logic |
| 11 | Fix sound buffer delete swap | Tiny | Crash |
| 12 | Add GL link status check | Small | Silent failure |
| 13 | Fix GL texture leak in resize | Small | Leak |
| 14 | Address sanitizer only in Debug | Small | Perf |
| 15 | Integrate or remove RHI layer | Large | Architecture |
| 16 | Cache uniform lookups | Medium | Performance |
| 17 | Fix `sprintf` → `snprintf` where needed | Small | Security |
| 18 | Enable frustum culling | Medium | Performance |
| 19 | Fix shallow model copy | Medium | Correctness |
| 20 | Replace `GLOB_RECURSE` with explicit source list | Small | Build hygiene |
| 21 | Write tests | Large | Quality |
