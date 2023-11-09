#ifndef ECPP_BITFIELD_SET_HPP_
#define ECPP_BITFIELD_SET_HPP_

#include <ecpp/bitfield_view.hpp>
namespace ecpp {

    namespace bf_impl {
        template<std::size_t Width> struct internal_storage_type;

        template<std::size_t Width>
            requires(Width <= 8)
        struct internal_storage_type<Width> : std::type_identity<std::uint8_t> {};

        template<std::size_t Width>
            requires(Width > 8 && Width <= 16)
        struct internal_storage_type<Width> : std::type_identity<std::uint16_t> {};

        template<std::size_t Width>
            requires(Width > 16 && Width <= 32)
        struct internal_storage_type<Width> : std::type_identity<std::uint32_t> {};


        template<std::size_t Width>
            requires(Width > 32 && Width <= 64)
        struct internal_storage_type<Width> : std::type_identity<std::uint64_t> {};

    } // namespace bf_impl

    template<is_bitfield_spec... Fields> class bitfield_set : public bitfield_set_view<bf_impl::internal_storage_type<11>, Fields...> {
        using storage_type = bf_impl::internal_storage_type<11>;
        constexpr bitfield_set() noexcept : bitfield_set_view<storage_type, Fields...>(m_storage) {
        }

      protected:
        storage_type m_storage;
    };


} // namespace ecpp

#endif
