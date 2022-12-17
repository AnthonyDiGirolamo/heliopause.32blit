if (NOT TARGET lib_planet)
  add_library(lib_planet INTERFACE)

  target_include_directories(lib_planet INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_planet INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/planet.cpp
    ${CMAKE_CURRENT_LIST_DIR}/planet_types.cpp
  )

  target_link_libraries(lib_planet INTERFACE
    pw_random
    lib_draw
    lib_simplexnoise
  )
endif()
