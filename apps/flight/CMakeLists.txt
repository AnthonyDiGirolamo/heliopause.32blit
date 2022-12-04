set(FLIGHT_NAME flight)

set(FLIGHT_SOURCE
  main.cpp
  main.hpp
  platform.cpp
  platform.hpp
  starfield.cpp
  starfield.hpp
)

set(FLIGHT_DISTRIBS LICENSE README.md)

blit_executable (${FLIGHT_NAME} ${FLIGHT_SOURCE})
blit_assets_yaml (${FLIGHT_NAME} ../planet_editor_assets.yml)
blit_metadata (${FLIGHT_NAME} ../planet_editor_metadata.yml)

# setup release packages
install (FILES ${FLIGHT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)

# Common libraries
target_link_libraries(${FLIGHT_NAME}
  lib_colors
  lib_draw
  lib_planet
  lib_random
  # lib_vec2
)

# Platform specific libs
if (PICO_BOARD STREQUAL "pimoroni_picosystem")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")
  target_link_libraries(${FLIGHT_NAME}
    pico_multicore
  )
endif()