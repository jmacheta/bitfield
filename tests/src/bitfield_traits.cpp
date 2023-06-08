#include <ecpp/bitfield.hpp>
#include <gtest/gtest.h>

#include <type_traits>
using namespace ecpp;

TEST(ECPP_Bitfield, Traits) {
    bf<unsigned, 1> f1;
    EXPECT_EQ(f1.mask, 1);
    EXPECT_FALSE(f1.is_signed);
    EXPECT_TRUE((std::is_same_v<unsigned, decltype(f1)::value_type>));
    EXPECT_TRUE((std::is_same_v<unsigned, decltype(f1)::underlying_type>));
    EXPECT_TRUE((std::is_same_v<unsigned, decltype(f1)::mask_type>));


    bf<int, 3> f2;
    EXPECT_EQ(f2.mask, 3);
    EXPECT_TRUE(f2.is_signed);
    EXPECT_TRUE((std::is_same_v<int, decltype(f2)::value_type>));
    EXPECT_TRUE((std::is_same_v<int, decltype(f2)::underlying_type>));
    EXPECT_TRUE((std::is_same_v<unsigned, decltype(f2)::mask_type>));


    enum E1 : int {};
    bf<E1, 7> f3;
    EXPECT_EQ(f3.mask, 7);
    EXPECT_TRUE(f2.is_signed);
    EXPECT_TRUE((std::is_same_v<E1, decltype(f3)::value_type>));
    EXPECT_TRUE((std::is_same_v<int, decltype(f3)::underlying_type>));
    EXPECT_TRUE((std::is_same_v<unsigned, decltype(f3)::mask_type>));


    enum E2 : std::uint8_t {};
    bf<E2, 15> f4;
    EXPECT_EQ(f4.mask, 15);
    EXPECT_FALSE(f4.is_signed);
    EXPECT_TRUE((std::is_same_v<E2, decltype(f4)::value_type>));
    EXPECT_TRUE((std::is_same_v<std::uint8_t, decltype(f4)::underlying_type>));
    EXPECT_TRUE((std::is_same_v<std::uint8_t, decltype(f4)::mask_type>));
}