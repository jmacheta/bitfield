#ifndef ECPP_BITMASK_HPP_
#define ECPP_BITMASK_HPP_

#include <bit>
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>

namespace ecpp {


    /// @brief Stores bitmask value and its properties
    template<std::unsigned_integral T> struct bitmask {
        using value_type = std::remove_cv_t<T>;

        constexpr explicit(false) bitmask(value_type value) noexcept : m_value{value} {
        }

        /**
         * Returns number of trailing zeros in bitmask value (counting from LSB)
         * @return Number of trailing zeros
         */
        [[nodiscard]] constexpr int trailing_zeros() const noexcept {
            return std::countr_zero(m_value);
        }

        /**
         * Returns number of leading zeros in bitmask value (counting from MSB)
         * @return Number of leading zeros or 0 when value is 0
         */
        [[nodiscard]] constexpr int leading_zeros() const noexcept {
            if (std::cmp_equal(m_value, 0U)) [[unlikely]] {
                return 0;
            }
            return std::countl_zero(m_value);
        }

        /**
         * Returns number of bits set in bitmask value
         * @return Number of bits set
         */
        [[nodiscard]] constexpr int popcount() const noexcept {
            return std::popcount(m_value);
        }

        /**
         * Returns width of bitmask value, i.e. distance between first and last set
         * bit
         * @return Width of bitmask
         */
        [[nodiscard]] constexpr int width() const noexcept {
            if (std::cmp_equal(m_value, 0U)) [[unlikely]] {
                return 0;
            }
            return std::numeric_limits<value_type>::digits - (trailing_zeros() + leading_zeros());
        }

        /**
         * Returns value of bitmask
         * @return value of bitmask
         */
        [[nodiscard]] constexpr value_type value() const noexcept {
            return m_value;
        }

        /**
         * Returns base value of bitmask, i.e. value with all trailing zeros removed
         * @return Base value of bitmask
         */
        [[nodiscard]] constexpr value_type base_value() const noexcept {
            return static_cast<value_type>(m_value >> trailing_zeros());
        }

        [[nodiscard]] constexpr operator value_type() const noexcept {
            return m_value;
        }


        [[nodiscard]] constexpr auto operator~() const noexcept {
            return bitmask(static_cast<value_type>(~m_value));
        }

        constexpr auto& operator=(value_type v) noexcept {
            return m_value = v;
        }

      private:
        value_type m_value; ///< Actual value of bitmask
    };


    namespace bf_impl {
        /// @brief Equals T when sizeof(T) >= sizeof(U) or U otherwise
        template<typename U, typename V> using larger_of_t = typename std::conditional_t<sizeof(V) < sizeof(U), U, V>;
    } // namespace bf_impl


    // Bitwise operators with bitmasks
    template<typename T, typename U> [[nodiscard]] constexpr auto operator|(bitmask<T> const& a, bitmask<U> const& b) noexcept {
        auto new_value = static_cast<bf_impl::larger_of_t<T, U>>(a.value() | b.value());
        return bitmask<decltype(new_value)>(new_value);
    }

    template<typename T, typename U> [[nodiscard]] constexpr auto operator&(bitmask<T> const& a, bitmask<U> const& b) noexcept {
        auto new_value = static_cast<bf_impl::larger_of_t<T, U>>(a.value() & b.value());
        return bitmask<decltype(new_value)>(new_value);
    }

    template<typename T, typename U> [[nodiscard]] constexpr auto operator^(bitmask<T> const& a, bitmask<U> const& b) noexcept {
        auto new_value = static_cast<bf_impl::larger_of_t<T, U>>(a.value() ^ b.value());
        return bitmask<decltype(new_value)>(new_value);
    }


    /**
     * @brief Checks if bitmask is contiguous, i.e. there are no 0 bits between 1 bits
     * @param a bitmask to check
     * @return true if bitmask is contiguous, false otherwise
     */
    template<typename T> [[nodiscard]] constexpr bool is_contiguous(bitmask<T> const& a) noexcept {
        if (std::cmp_equal(a.value(), 0U)) [[unlikely]] {
            return false;
        }
        return a.width() == a.popcount();
    }

    // CTAD
    template<std::signed_integral T> bitmask(T) -> bitmask<std::make_unsigned_t<T>>;
    template<std::unsigned_integral T> bitmask(T) -> bitmask<T>;


    /**
     * Concept true, when the T is a specialization of bitmask type
     */
    template<typename T>
    concept is_bitmask = requires {
        typename T::value_type;
        requires std::same_as<std::remove_cv_t<T>, bitmask<typename T::value_type>>;
    };


} // namespace ecpp
#endif
