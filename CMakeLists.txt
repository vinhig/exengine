cmake_minimum_required(VERSION 3.31)
project(HelloWorld)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/exengine/**.c)
file(GLOB_RECURSE PHYSFS_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/lib/physfs/**.c)

message("hello guys ${SRC_FILES}")

add_executable(exengine
        ${SRC_FILES}
        ${PHYSFS_FILES}
        ${PROJECT_SOURCE_DIR}/src/lib/MojoAL/mojoal.c

        ${PROJECT_SOURCE_DIR}/src/game.c
        ${PROJECT_SOURCE_DIR}/src/main.c
)

target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/src/inc)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/src/lib)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/src/lib/physfs)
target_include_directories(exengine PUBLIC ${PROJECT_SOURCE_DIR}/src/exengine)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

if(TARGET SDL2::SDL2main)
    target_link_libraries(exengine PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(exengine PRIVATE SDL2::SDL2 m)
target_compile_options(exengine PUBLIC -fsanitize=undefined)
target_compile_options(exengine PUBLIC -fsanitize=address,undefined -Werror -Wall)
target_link_options(exengine PUBLIC -fsanitize=address,undefined)