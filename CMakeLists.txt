cmake_minimum_required(VERSION 3.14)

project(ecpp_bitfield LANGUAGES CXX VERSION 0.9.0)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

add_library(ecpp_bitfield INTERFACE)

add_library(ecpp::bitfield ALIAS ecpp_bitfield)

target_include_directories(ecpp_bitfield INTERFACE include)

option(ECPP_BITFIELD_BUILD_TESTS "Build unit tests" OFF)
option(ECPP_BITFIELD_BUILD_EXAMPLES "Build examples" OFF)

if (PROJECT_IS_TOP_LEVEL OR ECPP_BITFIELD_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif ()



if (PROJECT_IS_TOP_LEVEL OR ECPP_BITFIELD_BUILD_EXAMPLES)
  enable_testing()
  add_subdirectory(examples)
endif ()