if (NOT TARGET lib_draw)
  add_library(lib_draw INTERFACE)

  target_include_directories(lib_draw INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_draw INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/draw.cpp
  )

  # target_link_libraries(lib_draw INTERFACE
  # )
endif()
