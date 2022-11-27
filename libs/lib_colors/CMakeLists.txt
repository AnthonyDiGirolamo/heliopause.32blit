if (NOT TARGET lib_colors)
  add_library(lib_colors INTERFACE)

  target_include_directories(lib_colors INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_colors INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/colors.cpp
  )

  # target_link_libraries(lib_colors INTERFACE
  # )
endif()
