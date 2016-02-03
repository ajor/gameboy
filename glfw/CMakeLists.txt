add_executable(gb main.cpp render_opengl.cpp)
target_link_libraries(gb gb_core)

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")

  target_link_libraries(gb "-s USE_GLFW=3")

else()

  find_package(OpenGL REQUIRED)
  if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(gb ${OPENGL_LIBRARIES})
  endif()

  find_package(GLEW REQUIRED)
  if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(gb ${GLEW_LIBRARIES})
  endif()

  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLFW REQUIRED glfw3)

  if (GLFW_FOUND)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_libraries(gb ${GLFW_LIBRARIES})
  endif()

endif()
