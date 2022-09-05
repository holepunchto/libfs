cmake_minimum_required(VERSION 3.22)

project(fs C)

add_subdirectory(vendor/libuv EXCLUDE_FROM_ALL)

add_library(fs OBJECT)

set_target_properties(
  fs
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE 1
)

target_sources(
  fs
  INTERFACE
    include/fs.h
  PRIVATE
    src/shared.c
)

target_include_directories(
  fs
  PUBLIC
    include
    $<TARGET_PROPERTY:uv,INCLUDE_DIRECTORIES>
)

add_library(fs_shared SHARED $<TARGET_OBJECTS:fs>)

set_target_properties(
  fs_shared
  PROPERTIES
  OUTPUT_NAME fs
)

target_link_libraries(
  fs_shared
  PUBLIC
    uv
)

add_library(fs_static STATIC $<TARGET_OBJECTS:fs>)

set_target_properties(
  fs_static
  PROPERTIES
  OUTPUT_NAME fs
)

target_link_libraries(
  fs_static
  PUBLIC
    uv_a
)

install(TARGETS fs_shared fs_static)

install(FILES include/fs.h DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test)
endif()
