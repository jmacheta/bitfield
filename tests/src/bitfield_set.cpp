#include "typed_test.hpp"

#include <ecpp/bitfield.hpp>


TEST(ecpp_bitfield_set, construction_from_spec) {
    using namespace ecpp;
    using f1 = bitfield_spec<int8_t, 0xF000'0000U>;
    using f2 = bitfield_spec<uint8_t, 0x0FF0'0000U>;
    using f3 = bitfield_spec<uint16_t, 0x000F'FFE0U>;
    using f4 = bitfield_spec<int16_t, 0x000'001FU>;

    uint32_t storage = 0xFFFFFFFF;
    auto     vset    = as_bitfield_set<f1, f2, f3, f4>(storage);
    EXPECT_EQ(vset.get<f1>(), -1);
    EXPECT_EQ(vset.get<f2>(), 0xFFU);
    EXPECT_EQ(vset.get<f3>(), 0x7FFFU);
    EXPECT_EQ(vset.get<f4>(), -1);
}