
#include "typed_test.hpp"

#include <ecpp/bitfield.hpp>

#include <algorithm>
#include <functional>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

TYPED_TEST_SUITE(ECPP_Bitfield, BitfieldTypes);

using namespace ecpp;


TEST(ECPP_BitfieldView, Construction) {
    std::uintmax_t storage = 0xFEEDFACEDEADBEEF;

    auto u8_hi = as_bitfield<std::uint8_t, 0xFF00U>(storage);
    auto u8_lo = as_bitfield<std::uint8_t, 0x00FFU>(storage);
    static_assert(!std::is_assignable_v<decltype(u8_hi), std::uint8_t>);
    EXPECT_EQ(u8_hi, 0xBE);
    EXPECT_EQ(u8_lo, 0xEF);

    auto u16_hi = as_bitfield<std::uint16_t, 0xFFFF0000U>(storage);
    auto u16_lo = as_bitfield<std::uint16_t, 0x0000FFFFU>(storage);
    EXPECT_EQ(u16_hi, 0xDEAD);
    EXPECT_EQ(u16_lo, 0xBEEF);

    auto u32_hi = as_bitfield<std::uint32_t, 0xFFFFFFFF00000000U>(storage);
    auto u32_lo = as_bitfield<std::uint32_t, 0x00000000FFFFFFFFU>(storage);
    EXPECT_EQ(u32_hi, 0xFEEDFACE);
    EXPECT_EQ(u32_lo, 0xDEADBEEF);

    auto u64 = as_bitfield<std::uint64_t, 0xFFFFFFFFFFFFFFFFU>(storage);
    EXPECT_EQ(u64, 0xFEEDFACEDEADBEEF);

    auto s8_hi1 = as_bitfield<std::int8_t, 0xFF00000000000000U>(storage);
    EXPECT_EQ(s8_hi1, -2); // signed equivalient of 0xFE

    auto s8_hi2 = as_bitfield<std::int8_t, 0x0FF0000000000000U>(storage);
    EXPECT_EQ(s8_hi2, -18); // signed equivalient of 0xEE

    auto s16_hi1 = as_bitfield<std::int16_t, 0x000000000FFFF000U>(storage);
    EXPECT_EQ(s16_hi1, -5413); // signed equivalient of 0xEADB

    auto s32 = as_bitfield<std::int32_t, 0x00001FFFFFFFE000U>(storage);
    EXPECT_EQ(s32, -696846995); // signed equivalient of 0xD676F56D

    auto s64 = as_bitfield<std::int64_t, 0x1FU>(storage);
    EXPECT_EQ(s64, 15);

    // Construction from spec
    using f1 = bitfield_spec<std::uint8_t, 0x3C00000000000000U>;
    auto specField = as_bitfield<f1>(storage);
    EXPECT_EQ(specField, 0xFU);
}

TYPED_TEST(ECPP_Bitfield, Construction_StorageCapabilities) {
    // std::uintmax_t storage;
    // using mask_type = typename std::make_unsigned_t<TypeParam>;

    // auto v = as_writable_bitfield<TypeParam, mask_type(-1U)>(storage);

    // constexpr static auto testValueMax  = TypeParam(-1);
    // constexpr static auto testValueZero = TypeParam(0);

    // v = testValueMax;
    // EXPECT_EQ(v, testValueMax);

    // v = testValueZero;
    // EXPECT_EQ(v, testValueZero);
}


// TYPED_TEST(ECPP_Bitfield, Construction_SingleBit) {
//     std::uint8_t storage8{};

//     auto v8_1 = as_writable_bitfield<TypeParam, 0b00000001U>(storage8);
//     auto v8_2 = as_writable_bitfield<TypeParam, 0b10000000U>(storage8);
//     auto v8_3 = as_writable_bitfield<TypeParam, 0b00010000U>(storage8);

//     auto testValueMax = TypeParam(typename bf8_1::underlying_type(bf8_1::is_signed ? 0 : 1));
//     auto testValueMin = TypeParam(typename bf8_1::underlying_type(bf8_1::is_signed ? -1 : 0));
//     auto testValueOvf = TypeParam(typename bf8_1::underlying_type(2));


//     v8_1       = testValueMax;
//     v8_2       = testValueMax;
//     v8_3       = testValueMax;

//     EXPECT_EQ(v8_1, testValueMax);
//     EXPECT_EQ(v8_2, testValueMax);
//     EXPECT_EQ(v8_3, testValueMax);

//     auto expectedStorageValue8_1 = bf8_1::is_signed ? 0b00000000U : 0b10010001U;
//     EXPECT_EQ(storage8, expectedStorageValue8_1);

//     v8_1 = testValueMin;
//     v8_2 = testValueMin;
//     v8_3 = testValueMin;

//     EXPECT_EQ(v8_1, testValueMin);
//     EXPECT_EQ(v8_2, testValueMin);
//     EXPECT_EQ(v8_3, testValueMin);

//     auto expectedStorageValue8_2 = bf8_1::field_traits::is_signed ? 0b10010001U : 0b00000000U;
//     EXPECT_EQ(storage8, expectedStorageValue8_2);

//     if constexpr (!bf8_1::field_traits::is_signed) {
//         v8_1 = testValueOvf;
//         v8_2 = testValueOvf;
//         v8_3 = testValueOvf;

//         // Value overflows bitfield, so loss in precision is expected
//         EXPECT_EQ(v8_1, testValueMin);
//         EXPECT_EQ(v8_2, testValueMin);
//         EXPECT_EQ(v8_3, testValueMin);


//         auto expectedStorageValue8_3 = 0b00000000U;
//         EXPECT_EQ(storage8, expectedStorageValue8_3);
//     }

//     // Now check on different storage type

//     std::uint16_t storage16{};
//     using bf16_1 = bitfield_view<TypeParam, 0b0000000000000001, decltype(storage16)>;
//     using bf16_2 = bitfield_view<TypeParam, 0b1000000000000000, decltype(storage16)>;
//     using bf16_3 = bitfield_view<TypeParam, 0b0000001000000000, decltype(storage16)>;

//     bf16_1 v16_1{storage16};
//     bf16_2 v16_2{storage16};
//     bf16_3 v16_3{storage16};

//     v16_1 = testValueMax;
//     v16_2 = testValueMax;
//     v16_3 = testValueMax;

//     EXPECT_EQ(v16_1, testValueMax);
//     EXPECT_EQ(v16_2, testValueMax);
//     EXPECT_EQ(v16_3, testValueMax);

//     auto expectedStorageValue16_1 = bf8_1::field_traits::is_signed ? 0b0000000000000000U : 0b1000001000000001U;
//     EXPECT_EQ(storage16, expectedStorageValue16_1);


//     v16_1 = testValueMin;
//     v16_2 = testValueMin;
//     v16_3 = testValueMin;

//     EXPECT_EQ(v16_1, testValueMin);
//     EXPECT_EQ(v16_2, testValueMin);
//     EXPECT_EQ(v16_3, testValueMin);

//     auto expectedStorageValue16_2 = bf8_1::field_traits::is_signed ? 0b1000001000000001U : 0b0000000000000000U;
//     EXPECT_EQ(storage16, expectedStorageValue16_2);
// }


// TYPED_TEST(ECPP_Bitfield, Construction_MultipleWidths) {
//     std::uint16_t storage16_4{};
//     using bf_4 = bitfield_view<TypeParam, 0b0001111000000000, decltype(storage16_4)>;
//     bf_4 v4{storage16_4};
//     auto testValueMax4 = TypeParam(typename bf_4::field_traits::underlying_type(bf_4::field_traits::is_signed ? 7 : 15));
//     auto testValueMin4 = TypeParam(typename bf_4::field_traits::underlying_type(bf_4::field_traits::is_signed ? -8 : 0));

//     v4 = testValueMax4;
//     EXPECT_EQ(v4, testValueMax4);
//     v4 = testValueMin4;
//     EXPECT_EQ(v4, testValueMin4);


//     std::uint16_t storage16_5{};
//     using bf_5 = bitfield<TypeParam, 0b0000011111000000, decltype(storage16_5)>;
//     bf_5 v5{storage16_5};
//     auto testValueMax5 = TypeParam(typename bf_5::field_traits::underlying_type(bf_5::field_traits::is_signed ? 15 : 31));
//     auto testValueMin5 = TypeParam(typename bf_5::field_traits::underlying_type(bf_5::field_traits::is_signed ? -16 : 0));

//     v5 = testValueMax5;
//     EXPECT_EQ(v5, testValueMax5);
//     v5 = testValueMin5;
//     EXPECT_EQ(v5, testValueMin5);

//     // Initialize storage with non-zero value
//     std::uint16_t storage16_5ne{0xFFFF};
//     using bf_5                         = bitfield_view<TypeParam, 0b0000011111000000, decltype(storage16_5)>;
//     std::uint16_t expectedStorageValue = 0b1111100000111111;
//     bf_5          v5ne{storage16_5ne};
//     v5ne = 0;
//     EXPECT_EQ(storage16_5ne, expectedStorageValue);


//     if (sizeof(TypeParam) > 1) {
//         std::uint16_t storage16_11{};
//         using bf_11 = bitfield_view<TypeParam, 0b1111111111100000, decltype(storage16_11)>;
//         bf_11 v11{storage16_11};
//         auto  testValueMax11 = TypeParam(typename bf_11::field_traits::underlying_type(bf_11::field_traits::is_signed ? 1023 : 2047));
//         auto  testValueMin11 = TypeParam(typename bf_11::field_traits::underlying_type(bf_11::field_traits::is_signed ? -1024 : 0));

//         v11 = testValueMax11;
//         EXPECT_EQ(v11, testValueMax11);
//         v11 = testValueMin11;
//         EXPECT_EQ(v11, testValueMin11);
//     }
// }

// inline int i() {
//     return 0;
// }