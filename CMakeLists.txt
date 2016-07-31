project(tinyRenderer)

cmake_minimum_required (VERSION 2.8.6)

# Version Number
set (SOFTWARE_RENDERER_VERSION_MAJOR 1)
set (SOFTWARE_RENDERER_VERSION_MINOR 0)
set (SOFTWARE_RENDERER_VERSION_PATCH 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS " -Wall -Wno-deprecated -std=c++11 -mwindows -mno-ms-bitfields -fopenmp")

if(DEFINED MINGW)
  configure_file("${PROJECT_SOURCE_DIR}/resources.rc.in" "${PROJECT_BINARY_DIR}/resources.rc")
  set(SOURCES ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/resources.rc)
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff -o <OBJECT> -i <SOURCE>")
  enable_language(RC)
endif(DEFINED MINGW)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}          # Generated .h files
  ${CMAKE_CURRENT_BINARY_DIR}  # For wrap/ui files
  )

set (SOURCES
  # project files
  ${SOURCES}
  main.cpp
  Mesh.cpp
  Images.cpp
  GL_Impl.cpp
  Utils.cpp
  Shaders.cpp
)

set(LIBS
  libgomp.a
  )

add_executable(renderer ${SOURCES})
target_link_libraries (renderer ${LIBS})
