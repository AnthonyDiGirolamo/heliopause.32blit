if (NOT TARGET lib_vec2)
  add_library(lib_vec2 INTERFACE)

  target_include_directories(lib_vec2 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  # target_sources(lib_vec2 INTERFACE
  #   ${CMAKE_CURRENT_LIST_DIR}/vec2.cpp
  # )

  # target_link_libraries(lib_vec2 INTERFACE
  # )
endif()
