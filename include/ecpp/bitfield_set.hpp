#ifndef ECPP_BITFIELD_SET_HPP_
#define ECPP_BITFIELD_SET_HPP_

#include <ecpp/bitfield.hpp>
namespace ecpp {
    namespace bf_impl {
        template<typename F, typename... Fields>
        concept one_of = std::disjunction_v<std::is_same<F, Fields>...>;
    }
    template<std::unsigned_integral Storage, is_bitfield_spec... Fields> class bitfield_set_view {
      public:
        using field_types  = std::tuple<Fields...>;
        using storage_type = Storage;

        constexpr bitfield_set_view(Storage& data) noexcept : m_data{data} {};

        template<typename F>
            requires(bf_impl::one_of<F, Fields...>)
        [[nodiscard]] constexpr auto get() noexcept {
            return as_writable_bitfield<F>(m_data);
        }

        template<typename F>
            requires(bf_impl::one_of<F, Fields...>)
        [[nodiscard]] constexpr auto get() const noexcept {
            return as_bitfield<F>(m_data);
        }

      protected:
        Storage& m_data;
    };


    template<is_bitfield_spec... Fields> constexpr auto as_bitfield_set(auto const& s) noexcept {
        return bitfield_set_view<std::remove_reference_t<decltype(s)>, Fields...>(s);
    }

    template<is_bitfield_spec... Fields> constexpr auto as_writable_bitfield_set(auto& s) noexcept {
        return bitfield_set_view<std::remove_reference_t<decltype(s)>, Fields...>(s);
    }


} // namespace ecpp

#endif
