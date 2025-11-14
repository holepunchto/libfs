cmake_minimum_required(VERSION 3.25)

find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)

project(fs C)

fetch_package("github:libuv/libuv@1.50.0")
fetch_package("github:holepunchto/libpath")

add_library(fs OBJECT)

set_target_properties(
  fs
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  fs
  INTERFACE
    include/fs.h
  PRIVATE
    src/access.c
    src/attr.c
    src/chmod.c
    src/close.c
    src/lock.c
    src/merge.c
    src/mkdir.c
    src/open.c
    src/platform.h
    src/read.c
    src/read-batch.c
    src/realpath.c
    src/rename.c
    src/rmdir.c
    src/sparse.c
    src/stat.c
    src/swap.c
    src/symlink.c
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
      src/mac/attr.c
      src/mac/lock.c
      src/mac/swap.c
      src/mac/trim.c
      src/mac/try-lock.c
      src/mac/unlock.c
  )
endif()

if(LINUX)
  target_sources(
    fs
    PRIVATE
      src/linux/attr.c
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
      src/win/attr.c
      src/win/lock.c
      src/win/nt.h
      src/win/sparse.c
      src/win/swap.c
      src/win/trim.c
      src/win/try-lock.c
      src/win/unlock.c
  )

  target_link_libraries(
    fs
    PUBLIC
      ntdll
  )
endif()

target_include_directories(
  fs
  PUBLIC
    include
    $<TARGET_PROPERTY:uv,INTERFACE_INCLUDE_DIRECTORIES>
  PRIVATE
    $<TARGET_PROPERTY:path,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(fs_shared SHARED)

set_target_properties(
  fs_shared
  PROPERTIES
  OUTPUT_NAME fs
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(
  fs_shared
  PUBLIC
    fs
    uv
  PRIVATE
    path_shared
)

add_library(fs_static STATIC)

set_target_properties(
  fs_static
  PROPERTIES
  OUTPUT_NAME fs
  PREFIX lib
)

target_link_libraries(
  fs_static
  PUBLIC
    fs
    uv_a
  PRIVATE
    path_static
)

install(TARGETS fs_shared fs_static)

install(FILES include/fs.h DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()

  add_subdirectory(test)
endif()
