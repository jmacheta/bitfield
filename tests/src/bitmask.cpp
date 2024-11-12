#include <array>
#include <ecpp/bitmask.hpp>
#include <gtest/gtest.h>
#include <ranges>
using BitmaskTypes = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

template <typename T> class Bitmask : public testing::Test {};

TYPED_TEST_SUITE(Bitmask, BitmaskTypes);

using namespace ecpp;

template <typename T> struct bitmask_test {
  T value;
  T expected_base_value;

  int expected_leading_zeros;
  int expected_trailing_zeros;
  int expected_popcount;
  int expected_width;
  bool is_contiguous;
};

TYPED_TEST(Bitmask, Construction) {
  using info = std::numeric_limits<TypeParam>;
  namespace views = std::views;
  auto max_width = info::digits;
  auto max = info::max();

  auto as_tp = [](auto x) { return static_cast<TypeParam>(x); };

  auto test_cases = std::to_array<bitmask_test<TypeParam>>({
      {as_tp(0), as_tp(0), max_width, max_width, 0, 0, true},
      {as_tp(1), as_tp(1), max_width - 1, 0, 1, 1, true},
      {as_tp(0b1010), as_tp(0b101), max_width - 4, 1, 2, 3, false},
      {as_tp(0b1110), as_tp(0b111), max_width - 4, 1, 3, 3, true},
      {as_tp(max), as_tp(max), 0, 0, max_width, max_width, true},
      {as_tp((max >> 1) - 1), as_tp(max >> 2), 1, 1, max_width - 2, max_width - 2, true},
  });

  for(auto const [index, v] : views::enumerate(test_cases)) {
    bitmask<TypeParam> b{v.value};

    EXPECT_EQ(b.value(), v.value) << "Invalid stored value at index " << index;
    EXPECT_EQ(b.base_value(), v.expected_base_value) << "Invalid base value at index " << index;
    EXPECT_EQ(b.leading_zeros(), v.expected_leading_zeros) << "Invalid leading zeros at index " << index;
    EXPECT_EQ(b.trailing_zeros(), v.expected_trailing_zeros) << "Invalid trailing zeros at index " << index;
    EXPECT_EQ(b.popcount(), v.expected_popcount) << "Invalid popcount at index " << index;
    EXPECT_EQ(b.width(), v.expected_width) << "Invalid width at index" << index;
    EXPECT_EQ(is_contiguous(b), v.is_contiguous) << "Invalid contiguity at index " << index;
  }
}

TYPED_TEST(Bitmask, Operators) {
  auto max_v = std::numeric_limits<TypeParam>::max();
  auto zero_v = std::numeric_limits<TypeParam>::min();
  auto high_v = static_cast<TypeParam>(max_v >> (std::numeric_limits<TypeParam>::digits / 2));
  auto low_v = static_cast<TypeParam>(max_v ^ high_v);

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

TEST(Bitmask, OperatorsOnDifferentTypes) {
  bitmask<std::uint8_t> a1{0b0011110000U};
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

TEST(Bitmask, type_deduction_guide) {
  auto from_signed = bitmask(0b10101010);
  auto from_unsigned = bitmask(0b10101010U);
  static_assert(std::is_same_v<decltype(from_signed), decltype(from_unsigned)>);

  auto from_int8 = bitmask(INT8_C(0b10101010));
  auto from_uint8 = bitmask(UINT8_C(0b10101010));
  static_assert(std::is_same_v<decltype(from_int8), decltype(from_uint8)>);
}
