#ifndef ECPP_BITFIELD_SET_HPP_
#define ECPP_BITFIELD_SET_HPP_

#include <ecpp/bitfield.hpp>
namespace ecpp {

    // template<has_bitfield_spec... FieldSpec> constexpr auto as_bitfield_set(auto
    // const& s) noexcept {
    //     return bitfield_set_view<std::remove_reference_t<decltype(s)>,
    //     FieldSpec...>(s);
    // }

    // template<has_bitfield_spec... FieldSpec> constexpr auto
    // as_writable_bitfield_set(auto& s) noexcept {
    //     return bitfield_set_view<std::remove_reference_t<decltype(s)>,
    //     FieldSpec...>(s);
    // }

    // template<std::size_t RequiredWidth> class minimal_storage_type;
    // template<std::size_t RequiredWidth> using minimal_storage_type_t = typename
    // minimal_storage_type<RequiredWidth>::type;

    // template<std::size_t RequiredWidth>
    //     requires((RequiredWidth > 0U) && (RequiredWidth <= 8U))
    // class minimal_storage_type<RequiredWidth> : public
    // std::type_identity<std::uint8_t> {}; template<std::size_t RequiredWidth>
    //     requires((RequiredWidth > 8U) && (RequiredWidth <= 16U))
    // class minimal_storage_type<RequiredWidth> : public
    // std::type_identity<std::uint16_t> {}; template<std::size_t RequiredWidth>
    //     requires((RequiredWidth > 16U) && (RequiredWidth <= 32U))
    // class minimal_storage_type<RequiredWidth> : public
    // std::type_identity<std::uint32_t> {}; template<std::size_t RequiredWidth>
    //     requires((RequiredWidth > 32U) && (RequiredWidth <= 64U))
    // class minimal_storage_type<RequiredWidth> : public
    // std::type_identity<std::uint32_t> {};

    // template<has_bitfield_spec FieldSpec, std::unsigned_integral StorageType =
    // minimal_storage_type_t<bitmask(FieldSpec::mask).width>> class bitfield :
    // public bitfield_view<FieldSpec, StorageType> {
    //     StorageType data;

    //   public:
    //     using typename bitfield_view<FieldSpec, StorageType>::value_type;
    //     using typename bitfield_view<FieldSpec, StorageType>::underlying_type;

    //     using bitfield_view<FieldSpec, StorageType>::operator=;

    //     constexpr bitfield() : bitfield_view<FieldSpec, StorageType>(data) {
    //     }
    //     constexpr bitfield(value_type initial_value) : bitfield_view<FieldSpec,
    //     StorageType>(data) {
    //         *this = initial_value;
    //     }
    // };

} // namespace ecpp

#endif
