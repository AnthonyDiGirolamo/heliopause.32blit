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

# Include 32Blit
find_package (32BLIT CONFIG REQUIRED PATHS ../32blit-sdk $ENV{PATH_32BLIT_SDK})

add_subdirectory(libs)
add_subdirectory(apps/planet_editor)
add_subdirectory(apps/flight)
