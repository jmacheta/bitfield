#include <ecpp/bitmask.hpp>
#include <gtest/gtest.h>


using BitmaskTypes = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;


template<typename T> class ECPP_Bitmask : public testing::Test {};


TYPED_TEST_SUITE(ECPP_Bitmask, BitmaskTypes);

using namespace ecpp;

template<typename T> static void construct_and_check(T value, unsigned expectedWidth, unsigned expectedShift, unsigned expectedOffset, T expectedBaseValue, bool shouldBeContinuous) {
    bitmask<T> m(value);
    EXPECT_EQ(m.value(), value);
    EXPECT_EQ(m.width(), expectedWidth) << "Invalid width for value " << std::hex << value;
    EXPECT_EQ(m.trailing_zeros(), expectedShift) << "Invalid trailing zeros for value " << std::hex << value;
    EXPECT_EQ(m.leading_zeros(), expectedOffset) << "Invalid leading zeros for value " << std::hex << value;
    EXPECT_EQ(m.base_value(), expectedBaseValue) << "Invalid base for value " << std::hex << value;
    EXPECT_EQ((m.popcount() == m.width()), shouldBeContinuous) << " " << std::hex << value;
}


TYPED_TEST(ECPP_Bitmask, ConstructionAndTraits) {
    constexpr static unsigned maxBitmaskBits = std::numeric_limits<unsigned char>::digits * sizeof(TypeParam);
    construct_and_check<TypeParam>(0, 0, maxBitmaskBits, 0, 0, true);

    // Check every continuous bitmask, i.e. 0b1, 0b11, 0b111, ...
    TypeParam mask = 0;
    for (unsigned width = 1; width != maxBitmaskBits; ++width) {
        mask = static_cast<TypeParam>(mask | static_cast<TypeParam>(TypeParam(1) << (width - 1)));
        // Shift bitmask left
        for (unsigned shift = 0; shift != maxBitmaskBits - width; ++shift) {
            auto m = static_cast<TypeParam>(mask << shift);
            construct_and_check<TypeParam>(m, width, shift, (maxBitmaskBits - (width + shift)), mask, true);
        }
    }

    // Check a few non-continuous bitmasks
    for (unsigned shift = 0; shift != (maxBitmaskBits - 3); ++shift) {
        construct_and_check<TypeParam>(static_cast<TypeParam>(static_cast<TypeParam>(0b101) << shift), 3, shift, (maxBitmaskBits - (3 + shift)), 0b101, false);
    }
    for (unsigned shift = 0; shift != (maxBitmaskBits - 7); ++shift) {
        construct_and_check<TypeParam>(static_cast<TypeParam>(static_cast<TypeParam>(0b1101101) << shift), 7, shift, (maxBitmaskBits - (7 + shift)), 0b1101101, false);
    }
}


TYPED_TEST(ECPP_Bitmask, Operators) {
    auto max_v  = std::numeric_limits<TypeParam>::max();
    auto zero_v = std::numeric_limits<TypeParam>::min();
    auto high_v = static_cast<TypeParam>(max_v >> (std::numeric_limits<TypeParam>::digits / 2));
    auto low_v  = static_cast<TypeParam>(max_v ^ high_v);

    bitmask zero{zero_v};
    bitmask max{max_v};
    bitmask high{high_v};
    bitmask low{low_v};
    {
        EXPECT_EQ((zero | zero), zero_v);
        EXPECT_EQ((zero | low), low_v);
        EXPECT_EQ((zero | high), high_v);
        EXPECT_EQ((zero | max), max_v);

        EXPECT_EQ((low | zero), low_v);
        EXPECT_EQ((low | low), low_v);
        EXPECT_EQ((low | high), max_v);
        EXPECT_EQ((low | max), max_v);

        EXPECT_EQ((high | zero), high_v);
        EXPECT_EQ((high | low), max_v);
        EXPECT_EQ((high | high), high_v);
        EXPECT_EQ((high | max), max_v);

        EXPECT_EQ((max | zero), max_v);
        EXPECT_EQ((max | low), max_v);
        EXPECT_EQ((max | high), max_v);
        EXPECT_EQ((max | max), max_v);
    }
    {
        EXPECT_EQ((zero ^ zero), zero_v);
        EXPECT_EQ((zero ^ low), low_v);
        EXPECT_EQ((zero ^ high), high_v);
        EXPECT_EQ((zero ^ max), max_v);

        EXPECT_EQ((low ^ zero), low_v);
        EXPECT_EQ((low ^ low), zero_v);
        EXPECT_EQ((low ^ high), max_v);
        EXPECT_EQ((low ^ max), high_v);

        EXPECT_EQ((high ^ zero), high_v);
        EXPECT_EQ((high ^ low), max_v);
        EXPECT_EQ((high ^ high), zero_v);
        EXPECT_EQ((high ^ max), low_v);

        EXPECT_EQ((max ^ zero), max_v);
        EXPECT_EQ((max ^ low), high_v);
        EXPECT_EQ((max ^ high), low_v);
        EXPECT_EQ((max ^ max), zero_v);
    }
    {
        EXPECT_EQ((zero & zero), zero_v);
        EXPECT_EQ((zero & low), zero_v);
        EXPECT_EQ((zero & high), zero_v);
        EXPECT_EQ((zero & max), zero_v);

        EXPECT_EQ((low & zero), zero_v);
        EXPECT_EQ((low & low), low_v);
        EXPECT_EQ((low & high), zero_v);
        EXPECT_EQ((low & max), low_v);

        EXPECT_EQ((high & zero), zero_v);
        EXPECT_EQ((high & low), zero_v);
        EXPECT_EQ((high & high), high_v);
        EXPECT_EQ((high & max), high_v);

        EXPECT_EQ((max & zero), zero_v);
        EXPECT_EQ((max & low), low_v);
        EXPECT_EQ((max & high), high_v);
        EXPECT_EQ((max & max), max_v);
    }
    {
        EXPECT_EQ((~zero), max_v);
        EXPECT_EQ((~low), high_v);
        EXPECT_EQ((~high), low_v);
        EXPECT_EQ((~max), zero_v);
    }
}


TEST(ECPP_Bitmask, OperatorsOnDifferentTypes) {
    bitmask<std::uint8_t>  a1{0b0011110000U};
    bitmask<std::uint16_t> a2{0b1000001111U};

    static_assert(std::is_same_v<decltype(a1 | a2), decltype(a2)>);
    static_assert(std::is_same_v<decltype(a1 ^ a2), decltype(a2)>);
    static_assert(std::is_same_v<decltype(a1 & a2), decltype(a2)>);

    EXPECT_EQ((a1 | a2), 0b1011111111);
    EXPECT_EQ((a1 ^ a2), 0b1011111111);
    EXPECT_EQ((a1 & a2), 0b0000000000);

    bitmask<std::uint16_t> b1(0x000FFF0U);
    bitmask<std::uint32_t> b2(0xF0FF00FU);

    static_assert(std::is_same_v<decltype(b1 | b2), decltype(b2)>);
    static_assert(std::is_same_v<decltype(b1 ^ b2), decltype(b2)>);
    static_assert(std::is_same_v<decltype(b1 & b2), decltype(b2)>);

    EXPECT_EQ((b1 | b2), 0xF0FFFFFU);
    EXPECT_EQ((b1 ^ b2), 0xF0F0FFFU);
    EXPECT_EQ((b1 & b2), 0x000F000U);
}


TEST(ECPP_Bitmask, is_contiguous) {
    EXPECT_FALSE(is_contiguous(bitmask(0)));
    EXPECT_FALSE(is_contiguous(bitmask(0b101)));
    EXPECT_FALSE(is_contiguous(bitmask(0b111110011)));

    EXPECT_TRUE(is_contiguous(bitmask(1)));
    EXPECT_TRUE(is_contiguous(bitmask(0b11)));
    EXPECT_TRUE(is_contiguous(bitmask(-1)));
    EXPECT_TRUE(is_contiguous(bitmask(0xFFFFFFFF)));
}