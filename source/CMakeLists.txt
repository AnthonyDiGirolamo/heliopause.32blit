set(PROJECT_SOURCE
  colors.cpp
  colors.hpp
  draw.cpp
  draw.hpp
  heliopause.cpp
  heliopause.hpp
  main.cpp
  main.hpp
  menu.cpp
  menu.hpp
  planet.cpp
  planet.hpp
  planet_editor.cpp
  planet_editor.hpp
  planet_types.hpp
  platform.cpp
  platform.hpp
)

set(PROJECT_DISTRIBS LICENSE README.md)

blit_executable (${PROJECT_NAME} ${PROJECT_SOURCE})
blit_assets_yaml (${PROJECT_NAME} ../assets.yml)
blit_metadata (${PROJECT_NAME} ../metadata.yml)
add_custom_target (flash DEPENDS ${PROJECT_NAME}.flash)

# setup release packages
install (FILES ${PROJECT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)

# Common libraries
add_subdirectory(lib_random)
add_subdirectory(lib_simplexnoise)
target_link_libraries(${PROJECT_NAME}
  lib_random
  lib_simplexnoise
  pw_string
)

# Platform specific libs
if (PICO_BOARD STREQUAL "pimoroni_picosystem")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")
  target_link_libraries(${PROJECT_NAME}
    pico_multicore
  )
endif()
