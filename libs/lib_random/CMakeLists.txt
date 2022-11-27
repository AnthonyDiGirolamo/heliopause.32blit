if (NOT TARGET lib_random)
  add_library(lib_random INTERFACE)

  target_include_directories(lib_random INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_sources(lib_random INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/random.cpp
  )

  target_link_libraries(lib_random INTERFACE
    pw_random
  )
endif()
