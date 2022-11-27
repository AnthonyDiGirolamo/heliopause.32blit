cmake_minimum_required(VERSION 3.16)

# Build configuration; approach this with caution!
if(MSVC)
  add_compile_options("/W4" "/wd4244" "/wd4324" "/wd4458" "/wd4100")
else()
  add_compile_options(
    # "-g"
    "-Wall" "-Wextra" "-Wdouble-promotion" "-Wno-unused-parameter")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)

project(heliopause)

# Include Pigweed
set(PW_ROOT "third_party/pigweed")
include(third_party/pigweed/pw_build/pigweed.cmake)
pw_set_backend(pw_assert pw_assert_basic)
pw_set_backend(pw_sys_io pw_sys_io_stdio)
add_subdirectory(third_party/pigweed pigweed)

set(PROJECT_SOURCE
  source/colors.cpp
  source/colors.hpp
  source/draw.cpp
  source/draw.hpp
  source/heliopause.cpp
  source/heliopause.hpp
  source/main.cpp
  source/main.hpp
  source/menu.cpp
  source/menu.hpp
  source/planet.cpp
  source/planet.hpp
  source/planet_editor.cpp
  source/planet_editor.hpp
  source/planet_types.hpp
  source/platform.cpp
  source/platform.hpp
)

set(PROJECT_DISTRIBS LICENSE README.md)

find_package (32BLIT CONFIG REQUIRED PATHS ../32blit-sdk $ENV{PATH_32BLIT_SDK})
blit_executable (${PROJECT_NAME} ${PROJECT_SOURCE})
blit_assets_yaml (${PROJECT_NAME} assets.yml)
blit_metadata (${PROJECT_NAME} metadata.yml)
add_custom_target (flash DEPENDS ${PROJECT_NAME}.flash)

# setup release packages
install (FILES ${PROJECT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)

add_subdirectory(source)

if (PICO_BOARD STREQUAL "pimoroni_picosystem")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")
  target_link_libraries(${PROJECT_NAME}
    pico_multicore
  )
endif()
