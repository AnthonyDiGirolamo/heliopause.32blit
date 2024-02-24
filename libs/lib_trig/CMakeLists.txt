if (NOT TARGET lib_trig)
  add_library(lib_trig INTERFACE)

  target_include_directories(lib_trig INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_trig INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/trig.cpp
  )

  # target_link_libraries(lib_trig INTERFACE
  # )
endif()
