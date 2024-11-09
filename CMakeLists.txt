cmake_minimum_required(VERSION 3.25)

project(ecpp_bitfield LANGUAGES CXX VERSION 0.9.0)

option(ECPP_BITFIELD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(ECPP_BITFIELD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})

add_library(ecpp_bitfield INTERFACE)
add_library(ecpp::bitfield ALIAS ecpp_bitfield)

target_compile_features(ecpp_bitfield INTERFACE cxx_std_20)
target_include_directories(ecpp_bitfield INTERFACE include)

if (ECPP_BITFIELD_TESTS)
  include(CTest)
  add_subdirectory(tests EXCLUDE_FROM_ALL)
endif ()

if (ECPP_BITFIELD_EXAMPLES)
  add_subdirectory(examples EXCLUDE_FROM_ALL)
endif ()
