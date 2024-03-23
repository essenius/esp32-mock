cmake_minimum_required(VERSION 3.14)

set(libName esp32-mock)

project(${libName} VERSION 0.1.5 LANGUAGES CXX)

# We only want to enable testing and code coverage for the top level project
# If we are called from a vcpkg build, we also don't want tests
if ((CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR) AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(TOP_LEVEL ON)
else()
  set(TOP_LEVEL OFF)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(esp32-mock)

message(STATUS "TOP_LEVEL: ${TOP_LEVEL}")
if(TOP_LEVEL)
  message(STATUS "Top level project - enabling tests")
  enable_testing()
  add_subdirectory(esp32-mock-test)
endif()