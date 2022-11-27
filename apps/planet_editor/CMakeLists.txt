set(PLANET_EDITOR_NAME planet_editor)

set(PLANET_EDITOR_SOURCE
  colors.cpp
  colors.hpp
  heliopause.cpp
  heliopause.hpp
  main.cpp
  main.hpp
  menu.cpp
  menu.hpp
  planet_editor.cpp
  planet_editor.hpp
  platform.cpp
  platform.hpp
)

set(PLANET_EDITOR_DISTRIBS LICENSE README.md)

blit_executable (${PLANET_EDITOR_NAME} ${PLANET_EDITOR_SOURCE})
blit_assets_yaml (${PLANET_EDITOR_NAME} ../planet_editor_assets.yml)
blit_metadata (${PLANET_EDITOR_NAME} ../planet_editor_metadata.yml)
add_custom_target (flash DEPENDS ${PLANET_EDITOR_NAME}.flash)

# setup release packages
install (FILES ${PLANET_EDITOR_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)

# Common libraries
target_link_libraries(${PLANET_EDITOR_NAME}
  lib_planet
  lib_random
  lib_simplexnoise
  pw_string
)

# Platform specific libs
if (PICO_BOARD STREQUAL "pimoroni_picosystem")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")
  target_link_libraries(${PLANET_EDITOR_NAME}
    pico_multicore
  )
endif()
