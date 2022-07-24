cmake_minimum_required(VERSION 3.16)

project(space-lion-example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD TRUE)

option(ExampleProjectUWP "Build DirectX UWP example project" OFF)
option(ExampleProject "Build OpenGL GLFW example project" OFF)

if(ExampleProjectUWP)
   set(USE_DX11 ON CACHE BOOL "Use DirectX11 graphics backend" FORCE)
else()
   set(USE_DX11 OFF CACHE BOOL "Use DirectX11 graphics backend" FORCE)
endif()

if(ExampleProject)
   set(USE_GL ON CACHE BOOL "Use OpenGL graphics backend" FORCE)
else()
   set(USE_GL OFF CACHE BOOL "Use OpenGL graphics backend" FORCE)
endif()

add_subdirectory(space-lion)

if(ExampleProject)
   add_subdirectory(ExampleProject)
endif()

if(ExampleProjectUWP)
   add_subdirectory(ExampleProjectUWP)
endif()