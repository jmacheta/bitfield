include(FetchContent)

FetchContent_Declare(
  googletest URL https://github.com/google/googletest/archive/main.zip FIND_PACKAGE_ARGS NAMES
                 GTest
)
FetchContent_MakeAvailable(googletest)

add_executable(ecpp_bitfield_ut src/bitfield_view_construction.cpp src/bitmask.cpp)
target_compile_features(ecpp_bitfield_ut PRIVATE cxx_std_23)
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

include(GoogleTest)
gtest_discover_tests(ecpp_bitfield_ut)
