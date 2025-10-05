cmake_minimum_required(VERSION 3.22)
project(HelloWorld)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/source/**.c)
file(GLOB_RECURSE PHYSFS_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/third_party/physfs/**.c)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED 23)

set(SDL_ATOMIC   ON CACHE BOOL "" FORCE)
set(SDL_AUDIO    ON CACHE BOOL "" FORCE)
set(SDL_VIDEO    ON CACHE BOOL "" FORCE)
set(SDL_RENDER   OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG   OFF CACHE BOOL "" FORCE)
set(SDL_EVENTS   ON CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_GAMEPAD  ON CACHE BOOL "" FORCE)
set(SDL_HAPTIC   OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI   OFF CACHE BOOL "" FORCE)
set(SDL_POWER    OFF CACHE BOOL "" FORCE)
set(SDL_THREADS  ON CACHE BOOL "" FORCE)
set(SDL_TIMERS   ON CACHE BOOL "" FORCE)
set(SDL_FILE     ON CACHE BOOL "" FORCE)
set(SDL_LOADSO   ON CACHE BOOL "" FORCE)
set(SDL_CPUINFO  ON CACHE BOOL "" FORCE)
set(SDL_FILESYSTEM ON CACHE BOOL "" FORCE)
set(SDL_STORAGE  ON CACHE BOOL "" FORCE)
set(SDL_SENSOR   OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA   OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/SDL3 EXCLUDE_FROM_ALL)

set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

add_executable(exengine
        ${SRC_FILES}
        ${PHYSFS_FILES}
        ${PROJECT_SOURCE_DIR}/third_party/MojoAL/mojoal.c
        ${PROJECT_SOURCE_DIR}/third_party/stb_vorbis.c
        ${PROJECT_SOURCE_DIR}/third_party/log/log.c
        ${PROJECT_SOURCE_DIR}/third_party/glad/glad.c

        ${PROJECT_SOURCE_DIR}/source/main.c
)

target_compile_definitions(exengine PRIVATE LOG_USE_COLOR)

target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/third_party/SDL3/include)

# find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
# find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

if(TARGET SDL2::SDL2main)
    target_link_libraries(exengine PRIVATE SDL3::SDL3main)
endif()

target_link_libraries(exengine PRIVATE SDL3::SDL3 m)
#target_compile_options(exengine PUBLIC -fsanitize=undefined)
#target_compile_options(exengine PUBLIC -fsanitize=address,undefined -Wall)
#target_link_options(exengine PUBLIC -fsanitize=address,undefined)