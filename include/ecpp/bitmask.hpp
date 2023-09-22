

#ifndef ECPP_BITMASK_HPP_
#define ECPP_BITMASK_HPP_
#pragma once
#include <bit>
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>
namespace ecpp {

    namespace bf_impl {
        /// @brief Equals T when sizeof(T) >= sizeof(U) or U otherwise
        template<typename U, typename V> using larger_of_t = typename std::conditional_t<sizeof(V) < sizeof(U), U, V>;
    } // namespace bf_impl

    /// @brief Stores bitmask value and its properties
    template<std::unsigned_integral T> struct bitmask {
        using value_type = std::remove_cv_t<T>;
        value_type value; ///< Actual value of bitmask

        constexpr explicit(false) bitmask(value_type v) noexcept : value{v} {
        }

        /**
         * Returns number of trailing zeros in bitmask value (counting from LSB)
         * @return Number of trailing zeros
         */
        constexpr int trailing_zeros() const noexcept {
            return std::countr_zero(value);
        }

        /**
         * Returns number of leading zeros in bitmask value (counting from MSB)
         * @return Number of leading zeros or 0 when value is 0
         */
        constexpr int leading_zeros() const noexcept {
            if (std::cmp_equal(value, 0U)) [[unlikely]] {
                return 0;
            }
            return std::countl_zero(value);
        }

        /**
         * Returns number of bits set in bitmask value
         * @return Number of bits set
         */
        constexpr int popcount() const noexcept {
            return std::popcount(value);
        }

        /**
         * Returns width of bitmask value, i.e. distance between first and last set bit
         * @return Width of bitmask
         */
        constexpr int width() const noexcept {
            if (std::cmp_equal(value, 0U)) [[unlikely]] {
                return 0;
            }
            return std::numeric_limits<value_type>::digits - (trailing_zeros() + leading_zeros());
        }

        /**
         * Returns base value of bitmask, i.e. value with all trailing zeros removed
         * @return Base value of bitmask
         */
        constexpr value_type base_value() const noexcept {
            return static_cast<value_type>(value >> trailing_zeros());
        }

        template<std::unsigned_integral U> [[nodiscard]] constexpr auto operator|(bitmask<U> v) const noexcept {
            return bitmask<bf_impl::larger_of_t<T, U>>(value | v.value);
        }

        template<std::unsigned_integral U> [[nodiscard]] constexpr auto operator&(bitmask<U> v) const noexcept {
            return bitmask<bf_impl::larger_of_t<T, U>>(value & v.value);
        }

        template<std::unsigned_integral U> [[nodiscard]] constexpr auto operator^(bitmask<U> v) const noexcept {
            return bitmask<bf_impl::larger_of_t<T, U>>(value ^ v.value);
        }

        [[nodiscard]] constexpr auto operator~() const noexcept {
            return bitmask(~value);
        }
    };


    // CTAD for aggregates
    template<class T> bitmask(T t) -> bitmask<T>;


} // namespace ecpp
#endif