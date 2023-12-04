
include(FetchContent)

cmake_policy(SET CMP0135 NEW)

FetchContent_Declare(
  googletest1 URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
                  FIND_PACKAGE_ARGS NAMES GTest
)
FetchContent_MakeAvailable(googletest1)

include(GoogleTest)

add_executable(ecpp_bitfield_ut src/bitfield_view_construction.cpp src/bitmask.cpp)

target_include_directories(ecpp_bitfield_ut PUBLIC include)
target_link_libraries(ecpp_bitfield_ut ecpp_bitfield GTest::gtest_main)
target_compile_options(ecpp_bitfield_ut PUBLIC -pthread)
target_link_options(ecpp_bitfield_ut PUBLIC -pthread)

target_compile_options(
  ecpp_bitfield_ut
  PUBLIC -Wall
         -Wextra
         -pedantic
         -Wconversion
         -Winit-self
         -Wcast-qual
         -Wcast-align
         -Wpacked
         -Wstrict-overflow=5
         -Wstrict-aliasing
         -fstrict-aliasing
         -Wredundant-decls
         -Wmissing-declarations
         -Wdisabled-optimization
         -Wold-style-cast
         -Wno-gnu-zero-variadic-macro-arguments
)

gtest_discover_tests(ecpp_bitfield_ut)

add_custom_command(
  TARGET ecpp_bitfield_ut COMMENT "Run tests" POST_BUILD COMMAND ctest ARGS --output-on-failure
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
