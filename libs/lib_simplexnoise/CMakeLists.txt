if (NOT TARGET lib_simplexnoise)
  add_library(lib_simplexnoise INTERFACE)

  target_include_directories(lib_simplexnoise INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_simplexnoise INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/SimplexNoise.cpp
  )

  # target_link_libraries(lib_simplexnoise INTERFACE
  # )
endif()
