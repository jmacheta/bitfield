

#ifndef ECPP_BITFIELD_HPP_
#define ECPP_BITFIELD_HPP_

#include <bit>
#include <climits>
#include <concepts>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>
namespace ecpp {

    /// @brief Concept is true, for types allowed to create bitfield from
    template<typename T>
    concept bitfield_compatible_type = (std::is_integral_v<T> || std::is_enum_v<T>);


    template<typename T>
    concept is_bitmask = requires(T t) {
                             t.value;
                             t.shift;
                             t.offset;
                             t.width;
                             t.base_value;
                             t.is_continuous;
                         };


    /// @brief Concept true, for types that contain required information to create bitfield from
    template<typename T>
    concept has_bitfield_spec = bitfield_compatible_type<typename T::value_type> && is_bitmask<decltype(T::mask)>;

    /// @brief  Concept true when the StorageType is wide enough to store bitfield with given FieldSpec
    template<typename StorageType, typename FieldSpec>
    concept storage_wide_enough_for = (FieldSpec::mask.width <= CHAR_BIT * sizeof(StorageType));


    /// @brief Equals T when sizeof(T) >= sizeof(U) or U otherwise
    template<typename U, typename V> using larger_of = typename std::conditional_t<sizeof(V) < sizeof(U), U, V>;


    /// @brief Stores bitmask value and its properties
    template<std::unsigned_integral T> struct bitmask {
        using value_type = std::remove_volatile_t<T>;
        value_type const value;         ///< Actual value of bitmask
        int const        shift;         ///< Distance between LSB and first bitmask's non-zero bit
        int const        offset;        ///< Distance between MSB and first bitmask's non-zero bit
        int const        width;         ///< Width of the bitmask - distance between most distant non-zero bits
        value_type const base_value;    ///< Bitmask value shifted right, so the LSB is non-zero
        bool const       is_continuous; ///< True when the mask has form {0+ zero-bits}{width non-zero bits}{shift zero-bits}

        /// @brief Constructs bitmask object from unsigned_integral value v
        explicit(false) constexpr bitmask(T v) noexcept :
            value{v},
            shift(std::countr_zero(v)),
            offset(std::countl_zero(v)),
            width((0U == v) ? 0 : static_cast<int>(sizeof(v)) * CHAR_BIT - (offset + shift)),
            base_value(value >> shift),
            is_continuous(width == std::popcount(v)) {
        }

        template<std::unsigned_integral U> [[nodiscard]] constexpr bitmask<larger_of<T, U>> operator|(bitmask<U> v) const noexcept {
            return value | v.value;
        }

        template<std::unsigned_integral U> [[nodiscard]] constexpr auto operator&(bitmask<U> v) const noexcept -> bitmask<larger_of<T, U>> {
            return value & v.value;
        }

        template<std::unsigned_integral U> [[nodiscard]] constexpr auto operator^(bitmask<U> v) const noexcept -> bitmask<larger_of<T, U>> {
            return value ^ v.value;
        }

        [[nodiscard]] constexpr bitmask operator~() const noexcept {
            return ~value;
        }
    };


    /// @brief Helper class to provide necessary information for bitfield creation
    template<bitfield_compatible_type FieldType, std::unsigned_integral auto Mask> struct bitfield_spec {
        using value_type = std::remove_cv_t<FieldType>;
        constexpr static bitmask mask{Mask};
    };


    template<has_bitfield_spec FieldSpec, std::unsigned_integral StorageType>
        requires storage_wide_enough_for<StorageType, FieldSpec>
    class bitfield_view {
      protected:
        StorageType& data;

      public:
        using value_type      = typename FieldSpec::value_type;                                                                                                  ///< Desired value type of the field
        using underlying_type = typename std::conditional_t<std::is_enum_v<value_type>, std::underlying_type<value_type>, std::type_identity<value_type>>::type; ///< value_type for integral types, or
                                                                                                                                                                 ///< underlying type of enum type
        constexpr static bitmask<std::remove_cv_t<StorageType>> mask{FieldSpec::mask.value};


        constexpr bitfield_view(StorageType& d) : data{d} {
            static_assert(mask.value != 0);
            static_assert(mask.is_continuous);
            static_assert(mask.width <= CHAR_BIT * sizeof(value_type));
        }


        [[nodiscard]] constexpr value_type value() const noexcept {
            if constexpr (!std::is_signed_v<underlying_type>) {
                return static_cast<value_type>(static_cast<StorageType>(data >> mask.shift) & mask.base_value);
            }
            auto v_shifted = static_cast<std::make_signed_t<StorageType>>(static_cast<StorageType>(data << mask.offset));
            return static_cast<value_type>(v_shifted >> (mask.offset + mask.shift));
        }

        [[nodiscard]] constexpr operator value_type() const noexcept {
            return value();
        }

        constexpr auto& operator=(value_type v) noexcept
            requires(!std::is_const_v<StorageType>)
        {
            auto current      = data; // Using temporary makes compiler to perform the second read earlier, when using volatile StorageType
            auto masked_value = static_cast<StorageType>(static_cast<StorageType>(static_cast<StorageType>(v) << mask.shift) & mask.value);
            data              = static_cast<StorageType>(masked_value | static_cast<StorageType>(current & (~mask).value));
            return *this;
        }

        constexpr auto& operator+=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = static_cast<value_type>(intermediate_value() + v);
        }

        constexpr auto& operator-=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = static_cast<value_type>(intermediate_value() - v);
        }

        constexpr auto& operator*=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this * v;
        }

        constexpr auto& operator/=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this / v;
        }

        constexpr auto& operator%=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this % v;
        }

        constexpr auto& operator&=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this & v;
        }

        constexpr auto& operator|=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this | v;
        }

        constexpr auto& operator^=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this ^ v;
        }

        constexpr auto& operator<<=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this << v;
        }

        constexpr auto& operator>>=(value_type v) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this >> v;
        }

        constexpr auto& operator++() noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this + 1;
        }

        constexpr value_type operator++(int) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            value_type tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr auto& operator--() noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            return *this = *this - 1;
        }

        constexpr value_type operator--(int) noexcept
            requires(std::integral<value_type> && std::assignable_from<StorageType&, value_type>)
        {
            value_type tmp = *this;
            --*this;
            return tmp;
        }

      private:
        constexpr auto intermediate_value() noexcept {
            return static_cast<std::remove_cv_t<StorageType>>(data >> mask.shift);
        }
    };

    template<bitfield_compatible_type FieldType, std::unsigned_integral auto Mask> constexpr auto as_bitfield(auto const& s) noexcept {
        return bitfield_view<bitfield_spec<FieldType, Mask>, std::remove_reference_t<decltype(s)>>(s);
    }

    template<has_bitfield_spec FieldSpec> constexpr auto as_bitfield(auto const& s) noexcept {
        return bitfield_view<FieldSpec, std::remove_reference_t<decltype(s)>>(s);
    }


    template<bitfield_compatible_type FieldType, std::unsigned_integral auto Mask> constexpr auto as_writable_bitfield(auto& s) noexcept {
        return bitfield_view<bitfield_spec<FieldType, Mask>, std::remove_reference_t<decltype(s)>>(s);
    }

    template<has_bitfield_spec FieldSpec> constexpr auto as_writable_bitfield(auto& s) noexcept {
        return bitfield_view<FieldSpec, std::remove_reference_t<decltype(s)>>(s);
    }


    template<class F, class... Fields>
    concept one_of = std::disjunction_v<std::is_same<F, Fields>...>;

    template<std::unsigned_integral Storage, has_bitfield_spec... Fields> class bitfield_set_view {
        Storage& data;

      public:
        using fields       = std::tuple<Fields...>;
        using storage_type = Storage;

        bitfield_set_view(Storage& d) : data{d} {};


        template<std::size_t I> [[nodiscard]] constexpr auto get() noexcept {
            using field = typename std::tuple_element_t<I, fields>;
            return as_writable_bitfield<field>(data);
        }

        template<std::size_t I> [[nodiscard]] constexpr auto get() const noexcept {
            using field = typename std::tuple_element_t<I, fields>;
            return as_bitfield<field>(data);
        }

        template<typename F>
            requires(one_of<F, Fields...>)
        [[nodiscard]] constexpr auto get() noexcept {
            return as_writable_bitfield<F>(data);
        }

        template<typename F>
            requires(one_of<F, Fields...>)
        [[nodiscard]] constexpr auto get() const noexcept {
            return as_bitfield<F>(data);
        }


        [[nodiscard]] storage_type as_integer() const noexcept {
            return data;
        }
    };


    template<has_bitfield_spec... FieldSpec> constexpr auto as_bitfield_set(auto const& s) noexcept {
        return bitfield_set_view<std::remove_reference_t<decltype(s)>, FieldSpec...>(s);
    }

    template<has_bitfield_spec... FieldSpec> constexpr auto as_writable_bitfield_set(auto& s) noexcept {
        return bitfield_set_view<std::remove_reference_t<decltype(s)>, FieldSpec...>(s);
    }


    template<std::size_t RequiredWidth> class minimal_storage_type;
    template<std::size_t RequiredWidth> using minimal_storage_type_t = typename minimal_storage_type<RequiredWidth>::type;

    template<std::size_t RequiredWidth>
        requires((RequiredWidth > 0U) && (RequiredWidth <= 8U))
    class minimal_storage_type<RequiredWidth> : public std::type_identity<std::uint8_t> {};
    template<std::size_t RequiredWidth>
        requires((RequiredWidth > 8U) && (RequiredWidth <= 16U))
    class minimal_storage_type<RequiredWidth> : public std::type_identity<std::uint16_t> {};
    template<std::size_t RequiredWidth>
        requires((RequiredWidth > 16U) && (RequiredWidth <= 32U))
    class minimal_storage_type<RequiredWidth> : public std::type_identity<std::uint32_t> {};
    template<std::size_t RequiredWidth>
        requires((RequiredWidth > 32U) && (RequiredWidth <= 64U))
    class minimal_storage_type<RequiredWidth> : public std::type_identity<std::uint32_t> {};


    template<has_bitfield_spec FieldSpec, std::unsigned_integral StorageType = minimal_storage_type_t<bitmask(FieldSpec::mask).width>> class bitfield : public bitfield_view<FieldSpec, StorageType> {
        StorageType data;

      public:
        using typename bitfield_view<FieldSpec, StorageType>::value_type;
        using typename bitfield_view<FieldSpec, StorageType>::underlying_type;

        using bitfield_view<FieldSpec, StorageType>::operator=;

        constexpr bitfield() : bitfield_view<FieldSpec, StorageType>(data) {
        }
        constexpr bitfield(value_type initial_value) : bitfield_view<FieldSpec, StorageType>(data) {
            *this = initial_value;
        }
    };


} // namespace ecpp
#endif
