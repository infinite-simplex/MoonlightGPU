cmake_minimum_required(VERSION 3.20)
project(MoonlightGPU VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(MOONLIGHT_BUILD_TESTS "Build unit tests (fetches GoogleTest)" ON)

add_library(moonlight_core STATIC
  src/trace.cpp
  src/trace_reader.cpp
  src/synthetic.cpp
  src/scoreboard.cpp
  src/pipeline.cpp
  src/scheduler.cpp
  src/coalescer.cpp
  src/cache.cpp
  src/stats.cpp
  src/sm.cpp
  src/simulator.cpp
)
target_include_directories(moonlight_core PUBLIC include)
if(MSVC)
  target_compile_options(moonlight_core PUBLIC /W4 /permissive- /utf-8)
else()
  target_compile_options(moonlight_core PUBLIC -Wall -Wextra -Wpedantic)
endif()

add_executable(moonlight src/main.cpp)
target_link_libraries(moonlight PRIVATE moonlight_core)

if(MOONLIGHT_BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_subdirectory(tests)
endif()
