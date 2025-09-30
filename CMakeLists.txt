cmake_minimum_required(VERSION 3.31)
project(HelloWorld)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/source/**.c)
file(GLOB_RECURSE PHYSFS_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/third_party/physfs/**.c)

add_executable(exengine
        ${SRC_FILES}
        ${PHYSFS_FILES}
        ${PROJECT_SOURCE_DIR}/third_party/MojoAL/mojoal.c
        ${PROJECT_SOURCE_DIR}/third_party/stb_vorbis.c
        ${PROJECT_SOURCE_DIR}/third_party/log/log.c
        ${PROJECT_SOURCE_DIR}/third_party/glad/glad.c

        ${PROJECT_SOURCE_DIR}/source/game.c
        ${PROJECT_SOURCE_DIR}/source/main.c
)

target_compile_definitions(exengine PRIVATE LOG_USE_COLOR)

target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/third_party)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

if(TARGET SDL2::SDL2main)
    target_link_libraries(exengine PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(exengine PRIVATE SDL2::SDL2 m)
target_compile_options(exengine PUBLIC -fsanitize=undefined)
target_compile_options(exengine PUBLIC -fsanitize=address,undefined -Wall)
target_link_options(exengine PUBLIC -fsanitize=address,undefined)