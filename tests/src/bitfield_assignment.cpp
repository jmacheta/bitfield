#include "ecpp/bitfield.hpp"
#include "ecpp/bitfield.hpp"
#include <gtest/gtest.h>
using namespace ecpp;

TEST(wtd, aa) {
using f1 = bf<int, 0b11000000000>;
using f2 = bf<unsigned, 0b111111>;


bitfield_set<unsigned, f1, f2> set;

auto set_f1 = set.get<f1>();
auto set_f2 = set.get<f2>();

set_f1 +=1;
set_f2 = 55;

EXPECT_EQ(set_f1, 1);
EXPECT_EQ(set_f2, 55);

}

inline int i() {
    return 0;
}