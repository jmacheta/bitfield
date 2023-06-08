#include "ecpp/bitfield.hpp"
#include "typed_test.hpp"

TYPED_TEST_SUITE(ECPP_Bitfield, BitfieldTypes);

template<typename T, std::uintmax_t shift = 0> void checkExtractOnSingleBit() {
    constexpr uintmax_t     mask = std::uintmax_t(1) << shift;
    ecpp::bitfield<T, mask> v{T{1}};
    EXPECT_EQ(v.extract(), mask);

    if constexpr (shift < (CHAR_BIT * sizeof(T) - 1)) {
        checkExtractOnSingleBit<T, shift + 1>();
    }
}

template<typename T, std::uintmax_t shift = 0> void checkExtractOnFourBits() {
    constexpr uintmax_t mask = std::uintmax_t(0xF) << shift;

    auto v1_ref  = std::uintmax_t(1) << shift;
    auto v4_ref1 = std::uintmax_t(0xE) << shift;
    auto v4_ref2 = std::uintmax_t(0xA) << shift;

    ecpp::bitfield<T, mask> v1{T{1}};
    EXPECT_EQ(v1.extract(), v1_ref);
    ecpp::bitfield<T, mask> v4_1{T{0xE}};
    EXPECT_EQ(v4_1.extract(), v4_ref1);

    ecpp::bitfield<T, mask> v4_2{T{0xA}};
    EXPECT_EQ(v4_2.extract(), v4_ref2);

    if constexpr ((shift + 4) < (CHAR_BIT * sizeof(T) - 1)) {
        checkExtractOnFourBits<T, shift + 4>();
    }
}

TYPED_TEST(ECPP_Bitfield, Access_Extract) {
    checkExtractOnSingleBit<TypeParam>();
    checkExtractOnFourBits<TypeParam>();
}


template<typename T, std::uintmax_t shift = 0> void checkDepositOnSingleBit() {
    constexpr uintmax_t     mask = std::uintmax_t(1) << shift;
    ecpp::bitfield<T, mask> v;
    T                       expected = static_cast<T>(mask);
    v.deposit(expected);
    if constexpr (std::is_unsigned_v<typename decltype(v)::underlying_type>) {
        EXPECT_EQ(v, 1);
    } else {
        EXPECT_EQ(v, -1);
    }


    if constexpr ((shift + 1) < (CHAR_BIT * sizeof(T))) {
        checkExtractOnSingleBit<T, shift + 1>();
    }
}

TYPED_TEST(ECPP_Bitfield, Access_Deposit) {
    checkDepositOnSingleBit<TypeParam>();
}