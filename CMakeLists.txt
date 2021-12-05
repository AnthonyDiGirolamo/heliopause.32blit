# Basic parameters; check that these match your project / environment
cmake_minimum_required(VERSION 3.9)

# Replace "game" with a name for your project (this is used the name of the output)
project(game)

set(PIGWEED_ROOT "third_party/pigweed")

# Add your sources here (adding headers is optional, but helps some CMake generators)
set(PROJECT_SOURCE
  source/SimplexNoise.cpp
  source/SimplexNoise.h
  source/colors.cpp
  source/colors.hpp
  source/draw.cpp
  source/draw.hpp
  source/game.cpp
  source/game.hpp
  source/menu.cpp
  source/menu.hpp
  source/planet_editor.cpp
  source/planet_editor.hpp
  source/planet.cpp
  source/planet.hpp
  source/planet_types.hpp
  source/random.cpp
  source/random.hpp
  ${PIGWEED_ROOT}/pw_string/format.cc
  ${PIGWEED_ROOT}/pw_string/string_builder.cc
  ${PIGWEED_ROOT}/pw_string/type_to_string.cc
  )

# ... and any other files you want in the release here
set(PROJECT_DISTRIBS LICENSE README.md)

# Build configuration; approach this with caution!
if(MSVC)
  add_compile_options("/W4" "/wd4244" "/wd4324" "/wd4458" "/wd4100")
else()
  add_compile_options("-Wall" "-Wextra" "-Wdouble-promotion" "-Wno-unused-parameter")
endif()

find_package (32BLIT CONFIG REQUIRED PATHS ../32blit-sdk $ENV{PATH_32BLIT_SDK})

blit_executable (${PROJECT_NAME} ${PROJECT_SOURCE})
blit_assets_yaml (${PROJECT_NAME} assets.yml)
blit_metadata (${PROJECT_NAME} metadata.yml)
add_custom_target (flash DEPENDS ${PROJECT_NAME}.flash)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${PIGWEED_ROOT}/pw_assert/public
  ${PIGWEED_ROOT}/pw_bytes/public
  ${PIGWEED_ROOT}/pw_polyfill/public
  ${PIGWEED_ROOT}/pw_polyfill/public_overrides
  ${PIGWEED_ROOT}/pw_polyfill/standard_library_public
  ${PIGWEED_ROOT}/pw_preprocessor/public
  ${PIGWEED_ROOT}/pw_random/public
  ${PIGWEED_ROOT}/pw_result/public
  ${PIGWEED_ROOT}/pw_span/public
  ${PIGWEED_ROOT}/pw_span/public_overrides
  ${PIGWEED_ROOT}/pw_status/public
  ${PIGWEED_ROOT}/pw_string/public
  )

# setup release packages
install (FILES ${PROJECT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)
