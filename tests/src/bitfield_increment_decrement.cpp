#include "ecpp/bitfield.hpp"
#include "typed_test.hpp"


// TYPED_TEST_SUITE_P(ECPP_Bitfield);


// TYPED_TEST_P(ECPP_Bitfield, Increment) {
//     ecpp::bitfield<TypeParam, 0b11000000> vPre;
//     ecpp::bitfield<TypeParam, 0b11000000> vPost;

//     // Check increment operators on some reasonable bitfield
//     typename decltype(vPre)::underlying_type expected{ecpp::numeric_limits<decltype(vPre)>::min()};
//     vPre  = static_cast<TypeParam>(expected);
//     vPost = static_cast<TypeParam>(expected);
//     while (expected != ecpp::numeric_limits<decltype(vPre)>::max()) {
//         EXPECT_EQ(vPre, vPost);
//         EXPECT_EQ(vPre, expected);
//         EXPECT_GT(++vPre, vPost++);
//         ++expected;
//     }

//     // Check whether the type is signed to avoid UB on signed overflow
//     if constexpr (std::is_unsigned_v<decltype(expected)>) {
//         EXPECT_LT(++vPre, vPost++); // ++vPre will overflow faster that vPost++
//         EXPECT_EQ(vPre, ecpp::numeric_limits<decltype(vPre)>::min());
//         EXPECT_EQ(vPre, vPost);
//     }
// }


// TYPED_TEST_P(ECPP_Bitfield, Decrement) {
//     ecpp::bitfield<TypeParam, 0b11000000> vPre;
//     ecpp::bitfield<TypeParam, 0b11000000> vPost;

//     // Check decrement operators on some reasonable bitfield
//     typename decltype(vPre)::underlying_type expected{ecpp::numeric_limits<decltype(vPre)>::max()};
//     vPre  = static_cast<TypeParam>(expected);
//     vPost = static_cast<TypeParam>(expected);

//     while (expected != ecpp::numeric_limits<decltype(vPre)>::min()) {
//         EXPECT_EQ(vPre, vPost);
//         EXPECT_EQ(vPre, expected);
//         EXPECT_LT(--vPre, vPost--);
//         --expected;
//     }

//     // Check whether the type is signed to avoid UB on signed underflow
//     if constexpr (std::is_unsigned_v<decltype(expected)>) {
//         EXPECT_GT(--vPre, vPost--); // --vPre will underflow faster that vPost--
//         EXPECT_EQ(vPre, ecpp::numeric_limits<decltype(vPre)>::max());
//         EXPECT_EQ(vPre, vPost);
//     }
// }

// REGISTER_TYPED_TEST_SUITE_P(ECPP_Bitfield, Increment, Decrement);
// INSTANTIATE_TYPED_TEST_SUITE_P(ECPP_ArithmeticBitfield, ECPP_Bitfield, BitfieldArithmeticTypes);