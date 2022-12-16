if (NOT TARGET lib_menu)
  add_library(lib_menu INTERFACE)

  target_include_directories(lib_menu INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_menu INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/menu.cpp
  )

  # target_link_libraries(lib_menu INTERFACE
  # )
endif()
