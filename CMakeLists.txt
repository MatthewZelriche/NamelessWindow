cmake_minimum_required(VERSION 3.19.0)
project(NamelessWindow CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin")

option(BUILD_NLSWIN_EXAMPLES "ON if you want to build examples, OFF to just build the shared library" OFF)

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

if (${BUILD_NLSWIN_EXAMPLES})
   add_subdirectory("${PROJECT_SOURCE_DIR}/examples/HelloWindow/")
endif()