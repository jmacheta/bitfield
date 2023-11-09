

#include <ecpp/bitfield_view.hpp>
#include <gtest/gtest.h>
#include <type_traits>

using namespace ecpp;

TEST(BitfieldView, Construction) {
    std::uintmax_t s1 = 0xFEEDFACEDEADBEEF;

    bitfield_view<decltype(s1), bitfield_spec<std::uint8_t, 0x0000'0000'0000'00FFU>>  u8(s1);
    bitfield_view<decltype(s1), bitfield_spec<std::uint32_t, 0x0000'0000'00FF'FF00U>> u16(s1);
    bitfield_view<decltype(s1), bitfield_spec<std::int32_t, 0x00FF'FFFF'FF00'0000U>>  i32(s1);
    bitfield_view<decltype(s1), bitfield_spec<std::int8_t, 0x7F00'0000'0000'0000U>>   i7(s1);
    bitfield_view<decltype(s1), bitfield_spec<bool, 0x8000'0000'0000'0000U>>          b(s1);

    EXPECT_EQ(u8, 0xEF);
    EXPECT_EQ(u16, 0xADBE);
    EXPECT_EQ(i32, -302330146);
    EXPECT_EQ(i7, -2);
    EXPECT_TRUE(b);

    std::uint8_t s2 = 0xAA;

    bitfield_view<decltype(s2), bitfield_spec<int, 0x0FU>>          f4(s2);
    bitfield_view<decltype(s2), bitfield_spec<std::uint8_t, 0x10U>> f1(s2);
    bitfield_view<decltype(s2), bitfield_spec<std::uint8_t, 0xE0U>> f3(s2);

    EXPECT_EQ(f4, -6);
    EXPECT_EQ(f1, 0);
    EXPECT_EQ(f3, 5);
}

TEST(BitfieldView, Factory) {
    std::uintmax_t storage = 0xFEED'FACE'DEAD'BEEF;

    auto u8_hi = as_bitfield<std::uint8_t, 0xFF00U>(storage);
    auto u8_lo = as_bitfield<std::uint8_t, 0x00FFU>(storage);
    static_assert(!std::is_assignable_v<decltype(u8_hi), std::uint8_t>);
    EXPECT_EQ(u8_hi, 0xBE);
    EXPECT_EQ(u8_lo, 0xEF);

    auto u16_hi = as_bitfield<std::uint16_t, 0xFFFF'0000U>(storage);
    auto u16_lo = as_bitfield<std::uint16_t, 0x0000'FFFFU>(storage);
    EXPECT_EQ(u16_hi, 0xDEAD);
    EXPECT_EQ(u16_lo, 0xBEEF);

    auto u32_hi = as_bitfield<std::uint32_t, 0xFFFF'FFFF'0000'0000U>(storage);
    auto u32_lo = as_bitfield<std::uint32_t, 0x0000'0000'FFFF'FFFFU>(storage);
    EXPECT_EQ(u32_hi, 0xFEEDFACE);
    EXPECT_EQ(u32_lo, 0xDEADBEEF);

    auto u64 = as_bitfield<std::uint64_t, 0xFFFF'FFFF'FFFF'FFFFU>(storage);
    EXPECT_EQ(u64, 0xFEEDFACEDEADBEEF);

    auto s8_hi1 = as_bitfield<std::int8_t, 0xFF00'0000'0000'0000U>(storage);
    EXPECT_EQ(s8_hi1, -2); // signed equivalient of 0xFE

    auto s8_hi2 = as_bitfield<std::int8_t, 0x0FF0'0000'0000'0000U>(storage);
    EXPECT_EQ(s8_hi2, -18); // signed equivalient of 0xEE

    auto s16_hi1 = as_bitfield<std::int16_t, 0x0000'0000'0FFF'F000U>(storage);
    EXPECT_EQ(s16_hi1, -5413); // signed equivalient of 0xEADB

    auto s32 = as_bitfield<std::int32_t, 0x0000'1FFF'FFFF'E000U>(storage);
    EXPECT_EQ(s32, -696846995); // signed equivalient of 0xD676F56D

    auto s64 = as_bitfield<std::int64_t, 0x1FU>(storage);
    EXPECT_EQ(s64, 15);

    // Construction from spec
    using f1       = bitfield_spec<std::uint8_t, 0x3C00'0000'0000'0000U>;
    auto specField = as_bitfield<f1>(storage);
    EXPECT_EQ(specField, 0xFU);

    volatile std::uintmax_t v_storage = 0xAAAA'BBBB'CCCC'DDDDU;

    auto vspecField = as_bitfield<f1>(v_storage);
    EXPECT_EQ(vspecField, 10);
}
