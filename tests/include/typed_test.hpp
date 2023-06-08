#ifndef BITFIELD_TYPED_TEST_HPP_
#define BITFIELD_TYPED_TEST_HPP_

#include <gtest/gtest.h>

#include <type_traits>


enum eU8 : std::uint8_t { EU8_0, EU8_1, EU8_MAX = UINT8_MAX };
enum eS8 : std::int8_t { ES8_0 = -1, ES8_1, ES8_MAX = INT8_MAX };
enum eU16 : std::uint16_t { EU16_0, EU16_1, EU16_MAX = UINT16_MAX };
enum eS16 : std::int16_t { ES16_0 = -1, ES16_1, ES16_MAX = INT16_MAX };
enum eU32 : std::uint32_t { EU32_0, EU32_1, EU32_MAX = UINT32_MAX };
enum eS32 : std::int32_t { ES32_0 = -1, ES32_1, ES32_MAX = INT32_MAX };
enum eU64 : std::uint64_t { EU64_0, EU64_1, EU64_MAX = UINT64_MAX };
enum eS64 : std::int64_t { ES64_0 = -1, ES64_1, ES64_MAX = INT64_MAX };


using BitmaskTypes = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, unsigned char, unsigned short, unsigned, unsigned long, unsigned long long>;

using BitfieldTypes = ::testing::Types<std::uint8_t,
                                       std::uint16_t,
                                       std::uint32_t,
                                       std::uint64_t,
                                       unsigned char,
                                       unsigned short,
                                       unsigned,
                                       unsigned long,
                                       unsigned long long,

                                       std::int8_t,
                                       std::int16_t,
                                       std::int32_t,
                                       std::int64_t,
                                       signed char,
                                       short,
                                       int,
                                       long,
                                       long long>;


using BitfieldArithmeticTypes = ::testing::Types<std::uint8_t,
                                                 std::uint16_t,
                                                 std::uint32_t,
                                                 std::uint64_t,
                                                 unsigned char,
                                                 unsigned short,
                                                 unsigned,
                                                 unsigned long,
                                                 unsigned long long,

                                                 std::int8_t,
                                                 std::int16_t,
                                                 std::int32_t,
                                                 std::int64_t,
                                                 signed char,
                                                 short,
                                                 int,
                                                 long,
                                                 long long>;


using BitfieldEnumTypes = ::testing::Types<eU8, eU16, eU32, eU64, eS8, eS16, eS32, eS64>;

template<typename T> class ECPP_Bitfield : public testing::Test {};

template<typename T> class ECPP_BitfieldProxy : public testing::Test {};

template<typename T> class ECPP_Bitmask : public testing::Test {};


#endif