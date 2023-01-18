set(ZSOLITAIRE_NAME zsolitaire)

set(ZSOLITAIRE_SOURCE
  main.cpp
  assets/card_font.cpp
  assets/card_font.hpp
)

set(ZSOLITAIRE_DISTRIBS LICENSE README.md)

blit_executable (${ZSOLITAIRE_NAME} ${ZSOLITAIRE_SOURCE})
blit_assets_yaml (${ZSOLITAIRE_NAME} assets.yml)
blit_metadata (${ZSOLITAIRE_NAME} metadata.yml)

# setup release packages
install (FILES ${ZSOLITAIRE_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)

# Common libraries
target_link_libraries(${ZSOLITAIRE_NAME}
  lib_colors
  lib_draw
  lib_menu
  lib_random
  pw_string
)

target_include_directories(${ZSOLITAIRE_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/assets
)

# Platform specific libs
if (PICO_BOARD STREQUAL "pimoroni_picosystem")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")
endif()
