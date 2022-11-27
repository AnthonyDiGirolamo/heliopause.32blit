if (NOT TARGET simplexnoise)
  add_library(simplexnoise INTERFACE)

  target_include_directories(simplexnoise INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(simplexnoise INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/SimplexNoise.cpp
  )

  # target_link_libraries(simplexnoise INTERFACE
  # )
endif()
