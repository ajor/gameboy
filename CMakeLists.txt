cmake_minimum_required(VERSION 2.8.12)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

project(Gameboy)

add_compile_options("-std=c++14")
add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-Wundef")
#add_compile_options("-Wshadow")
add_compile_options("-Wpointer-arith")
add_compile_options("-Wcast-align")
add_compile_options("-Wwrite-strings")
add_compile_options("-Wcast-qual")
add_compile_options("-Wswitch-default")
add_compile_options("-Wswitch-enum")
#add_compile_options("-Wconversion")
add_compile_options("-Wunreachable-code")
add_compile_options("-Wformat=2")
add_compile_options("-Wstrict-overflow=5")
add_compile_options("-Wdisabled-optimization")
#add_compile_options("-pedantic")

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(core)

add_executable(gb main.cpp render_opengl.cpp)
target_link_libraries(gb gb_core)

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
