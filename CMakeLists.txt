cmake_minimum_required(VERSION 3.22)
project(HelloWorld)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/source/**.c)
file(GLOB_RECURSE PHYSFS_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/third_party/physfs/**.c)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED 23)

set(SDL_ATOMIC   ON)
set(SDL_AUDIO    ON)
set(SDL_VIDEO    ON)
set(SDL_RENDER   OFF)
set(SDL_DIALOG   OFF)
set(SDL_EVENTS   ON)
set(SDL_JOYSTICK OFF)
set(SDL_GAMEPAD  OFF)
set(SDL_HAPTIC   OFF)
set(SDL_HIDAPI   OFF)
set(SDL_POWER    OFF)
set(SDL_THREADS  ON)
set(SDL_TIMERS   ON)
set(SDL_FILE     ON)
set(SDL_LOADSO   ON)
set(SDL_CPUINFO  OFF)
set(SDL_FILESYSTEM ON)
set(SDL_STORAGE  ON)
set(SDL_SENSOR   OFF)
set(SDL_CAMERA   OFF)
set(SDL_WAYLAND  ON)
set(SDL_VULKAN   OFF)

set(SDL_TEST_LIBRARY OFF)
set(SDL_TESTS OFF)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

add_library(imgui
        third_party/imgui/cimgui.cpp
        third_party/imgui/cimgui_impl.cpp
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_demo.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/imgui_impl_sdl3.cpp
        third_party/imgui/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC third_party/imgui/ third_party/cimgui/ third_party/ third_party/SDL3/include)
target_compile_definitions(imgui PUBLIC "-DIMGUI_IMPL_API=extern \"C\" ")
target_compile_definitions(imgui PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL3)

add_subdirectory(third_party/SDL3 EXCLUDE_FROM_ALL)

add_library(mojoal
        ${PROJECT_SOURCE_DIR}/third_party/MojoAL/mojoal.c
)
target_include_directories(mojoal PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(mojoal PUBLIC ${PROJECT_SOURCE_DIR}/third_party/SDL3/include)

add_executable(exengine
        ${SRC_FILES}
        ${PHYSFS_FILES}
        ${PROJECT_SOURCE_DIR}/third_party/stb_vorbis.c
        ${PROJECT_SOURCE_DIR}/third_party/log/log.c
        ${PROJECT_SOURCE_DIR}/third_party/glad/glad.c

        ${PROJECT_SOURCE_DIR}/source/main.c
)

target_compile_definitions(exengine PRIVATE LOG_USE_COLOR AL_LIBTYPE_STATIC)

target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/third_party/SDL3/include)

target_link_libraries(exengine PRIVATE SDL3::SDL3-static m mojoal imgui)
#target_compile_options(exengine PUBLIC -fsanitize=undefined)
target_compile_options(exengine PUBLIC -Wall -Werror)
target_compile_definitions(exengine PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL3)
#target_compile_options(exengine PUBLIC -fsanitize=address,undefined -Wall)
#target_link_options(exengine PUBLIC -fsanitize=address,undefined)
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
 add_compile_options(-static)
 add_link_options(-static -static-libgcc -static-libstdc++)
endif ()