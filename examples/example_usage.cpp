#include <ecpp/bitfield_set.hpp>

using namespace ecpp;

struct xPSR {
  static inline auto address = reinterpret_cast<uint32_t volatile *>(0x20000);

  static inline auto ISR = as_writable_bitfield<unsigned, 0x0000'FFFFU>(*address);
  static inline auto T = as_writable_bitfield<bool, 0x0100'0000U>(*address);
  static inline auto V = as_writable_bitfield<bool, 0x1000'0000U>(*address);
  static inline auto C = as_writable_bitfield<bool, 0x2000'0000U>(*address);
  static inline auto Z = as_writable_bitfield<bool, 0x4000'0000U>(*address);
  static inline auto N = as_writable_bitfield<bool, 0x8000'0000U>(*address);
};

int main() {
  using version = bitfield_spec<uint8_t, 0b11110000>;
  using traffic_class = bitfield_spec<int, 0b00001111>;

  uint8_t x = 0xFF;

  auto first_byte = as_writable_bitfield_set<version, traffic_class>(x);
  // xPSR()->get<xPSR::ISR>();
  xPSR::ISR += 1;

  auto v = first_byte.get<version>();

  return x;
}
