#include <ecpp/bitfield_set.hpp>

using namespace ecpp;


int main() {
    using version       = bitfield_spec<uint8_t, 0b11110000>;
    using traffic_class = bitfield_spec<int, 0b00001111>;

    uint8_t x = 0xFF;

    auto first_byte = as_writable_bitfield_set<version, traffic_class>(x);

    auto v = first_byte.get<version>();

    return x;
}