list(APPEND tests
  attr-get
  attr-list
  mkdir-rmdir
  mkdir-rmdir-recursive
  open-close
  read
  read-batch
  stat
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(${test} fs_shared)

  add_test(
    NAME ${test}
    COMMAND ${test}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endforeach()
