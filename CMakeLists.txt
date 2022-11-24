cmake_minimum_required(VERSION 3.22)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

project(fs C)

if(NOT TARGET uv)
  add_subdirectory(vendor/libuv EXCLUDE_FROM_ALL)
endif()

if(NOT TARGET path)
  add_subdirectory(vendor/libpath EXCLUDE_FROM_ALL)
endif()

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
    src/access.c
    src/chmod.c
    src/close.c
    src/lock.c
    src/mkdir.c
    src/open.c
    src/platform.h
    src/read.c
    src/read-batch.c
    src/realpath.c
    src/rmdir.c
    src/sparse.c
    src/stat.c
    src/swap.c
    src/trim.c
    src/truncate.c
    src/try-lock.c
    src/unlink.c
    src/unlock.c
    src/write.c
    src/write-batch.c
)

if(UNIX)
  target_sources(
    fs
    PRIVATE
      src/unix/lock.c
      src/unix/sparse.c
  )
endif()

if(APPLE)
  target_sources(
    fs
    PRIVATE
      src/mac/lock.c
      src/mac/swap.c
      src/mac/trim.c
      src/mac/try-lock.c
      src/mac/unlock.c
  )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_sources(
    fs
    PRIVATE
      src/linux/lock.c
      src/linux/swap.c
      src/linux/trim.c
      src/linux/try-lock.c
      src/linux/unlock.c
  )
endif()

if(WIN32)
  target_sources(
    fs
    PRIVATE
      src/win/lock.c
      src/win/sparse.c
      src/win/swap.c
      src/win/trim.c
      src/win/try-lock.c
      src/win/unlock.c
  )
endif()

target_include_directories(
  fs
  PUBLIC
    include
    $<TARGET_PROPERTY:uv,INCLUDE_DIRECTORIES>
  PRIVATE
    $<TARGET_PROPERTY:path,INCLUDE_DIRECTORIES>
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
  PRIVATE
    path_shared
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
    uv
  PRIVATE
    path_static
)

install(TARGETS fs_shared fs_static)

install(FILES include/fs.h DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test)
endif()
