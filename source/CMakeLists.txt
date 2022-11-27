# Common libraries
add_subdirectory(lib_random)
add_subdirectory(lib_simplexnoise)
target_link_libraries(${PROJECT_NAME}
  lib_random
  lib_simplexnoise
  pw_string
)
