#include "typed_test.hpp"

#include <ecpp/bitfield.hpp>

TYPED_TEST_SUITE(ECPP_Bitmask, BitmaskTypes);

using namespace ecpp;

template<typename T> static void construct_and_check(T value, unsigned expectedWidth, unsigned expectedShift, unsigned expectedOffset, T expectedBaseValue, bool shouldBeContinuous) {
    bitmask<T> m(value);
    EXPECT_EQ(m.value, value);
    EXPECT_EQ(m.width, expectedWidth) << "Invalid width for value " << std::hex << value;
    EXPECT_EQ(m.shift, expectedShift) << "Invalid shift for value " << std::hex << value;
    EXPECT_EQ(m.offset, expectedOffset) << "Invalid offset for value " << std::hex << value;
    EXPECT_EQ(m.base_value, expectedBaseValue) << "Invalid base_value for value " << std::hex << value;
    EXPECT_EQ(m.is_continuous, shouldBeContinuous) << "not expected continuity for value " << std::hex << value;
}


TYPED_TEST(ECPP_Bitmask, ConstructionAndTraits) {
    constexpr static unsigned maxBitmaskBits = std::numeric_limits<unsigned char>::digits * sizeof(TypeParam);
    construct_and_check<TypeParam>(0, 0, maxBitmaskBits, maxBitmaskBits, 0, true);

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
    bitmask<TypeParam> a1(0b11110000U);
    bitmask<TypeParam> a2(0b00001111U);

    EXPECT_EQ((a1 | a2).value, 0b11111111U);
    EXPECT_EQ((a1 ^ a2).value, 0b11111111U);
    EXPECT_EQ((a1 & a2).value, 0b00000000U);

    bitmask<TypeParam> b1(0b11111000U);
    bitmask<TypeParam> b2(0b00001111U);
    EXPECT_EQ((b1 | b2).value, 0b11111111);
    EXPECT_EQ((b1 ^ b2).value, 0b11110111);
    EXPECT_EQ((b1 & b2).value, 0b00001000);

    bitmask<TypeParam> n1(0U);
    EXPECT_EQ((~n1).value, TypeParam(-1));
    bitmask<TypeParam> n2(1U);
    EXPECT_EQ((~n2).value, TypeParam(-2));
}


TEST(ECPP_Bitmask, OperatorsOnDifferentTypes) {
    bitmask<std::uint8_t>  a1(0b0011110000U);
    bitmask<std::uint16_t> a2(0b1000001111U);

    static_assert(std::is_same_v<decltype(a1 | a2), decltype(a2)>);
    static_assert(std::is_same_v<decltype(a1 ^ a2), decltype(a2)>);
    static_assert(std::is_same_v<decltype(a1 & a2), decltype(a2)>);

    EXPECT_EQ((a1 | a2).value, 0b1011111111);
    EXPECT_EQ((a1 ^ a2).value, 0b1011111111);
    EXPECT_EQ((a1 & a2).value, 0b0000000000);

    bitmask<std::uint16_t> b1(0x000FFF0U);
    bitmask<std::uint32_t> b2(0xF0FF00FU);

    static_assert(std::is_same_v<decltype(b1 | b2), decltype(b2)>);
    static_assert(std::is_same_v<decltype(b1 ^ b2), decltype(b2)>);
    static_assert(std::is_same_v<decltype(b1 & b2), decltype(b2)>);

    EXPECT_EQ((b1 | b2).value, 0xF0FFFFFU);
    EXPECT_EQ((b1 ^ b2).value, 0xF0F0FFFU);
    EXPECT_EQ((b1 & b2).value, 0x000F000U);
}