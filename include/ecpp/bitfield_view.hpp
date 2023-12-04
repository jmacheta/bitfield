
#ifndef ECPP_BITFIELD_HPP_
#define ECPP_BITFIELD_HPP_
#include <ecpp/bitmask.hpp>

#include <cstdint>
#include <tuple>
namespace ecpp {
/// @brief Concept is true, for types allowed to create bitfield from
template <typename T>
concept bitfield_compatible_type = (std::is_integral_v<T> || std::is_enum_v<T>);

namespace bf_impl {
template <typename T> struct underlying_type;
template <std::integral T> struct underlying_type<T> : public std::type_identity<T> {};
template <typename T>
  requires(std::is_enum_v<T>)
struct underlying_type<T> : public std::underlying_type<T> {};

template <class T> using underlying_type_t = typename underlying_type<T>::type;

template <typename T> struct has_signed_representation;
template <typename T>
  requires(std::is_unsigned_v<underlying_type_t<T>>)
struct has_signed_representation<T> : public std::false_type {};
template <typename T>
  requires(std::is_signed_v<underlying_type_t<T>>)
struct has_signed_representation<T> : public std::true_type {};
template <class T>
inline constexpr bool has_signed_representation_v = has_signed_representation<T>::value;

template <typename T> struct make_unsigned;
template <typename T>
  requires(std::is_unsigned_v<underlying_type_t<T>>)
struct make_unsigned<T> : public std::type_identity<underlying_type_t<T>> {};
template <typename T>
  requires(std::is_signed_v<underlying_type_t<T>>)
struct make_unsigned<T> : public std::make_unsigned<underlying_type_t<T>> {};

template <class T> using make_unsigned_t = typename make_unsigned<T>::type;

} // namespace bf_impl

template <std::uintmax_t Value, typename T>
concept fits_in = Value <= std::numeric_limits<bf_impl::make_unsigned_t<T>>::max();

/// @brief Helper class to provide necessary information for bitfield creation
template <bitfield_compatible_type T, std::uintmax_t MaskValue>
  requires(fits_in<bitmask(MaskValue).base_value(), T>)
struct bitfield_spec {
  using value_type = std::remove_cv_t<T>;
  constexpr static bitmask mask{MaskValue};
};

/// @brief Concept true, for types that contain required information to create
/// bitfield from
template <typename T>
concept is_bitfield_spec =
    bitfield_compatible_type<typename T::value_type> && is_bitmask<decltype(T::mask)>;

template <std::unsigned_integral StorageType, is_bitfield_spec Spec>
  requires(fits_in<Spec::mask, StorageType>)
class bitfield_view {
protected:
  StorageType &data;

public:
  using storage_type = StorageType;
  using value_type = typename Spec::value_type; ///< Desired value type of the field

  constexpr static bitmask<std::remove_cv_t<storage_type>> mask{
      static_cast<storage_type>(Spec::mask)};

  constexpr bitfield_view(storage_type &d) : data{d} {
    static_assert(is_contiguous(mask), "bitfield mask must be contiguous");
  }

  [[nodiscard]] constexpr value_type value() const noexcept {
    using namespace bf_impl;
    if constexpr(!has_signed_representation_v<value_type>) {
      return static_cast<value_type>(static_cast<storage_type>(data >> mask.trailing_zeros()) &
                                     mask.base_value());
    }
    auto v_shifted = static_cast<std::make_signed_t<storage_type>>(
        static_cast<storage_type>(data << mask.leading_zeros()));
    return static_cast<value_type>(v_shifted >> (mask.leading_zeros() + mask.trailing_zeros()));
  }

  [[nodiscard]] constexpr operator value_type() const noexcept { return value(); }

  constexpr auto &operator=(value_type v) noexcept
    requires(!std::is_const_v<storage_type>)
  {
    auto current = data; // Using temporary makes compiler to perform the second read
                         // earlier, when using volatile storage_type
    auto masked_value = static_cast<storage_type>(
        static_cast<storage_type>(static_cast<storage_type>(v) << mask.trailing_zeros()) &
        mask.value());
    data = static_cast<storage_type>(masked_value |
                                     static_cast<storage_type>(current & (~mask).value()));
    return *this;
  }

  constexpr auto &operator+=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = static_cast<value_type>(intermediate_value() + static_cast<storage_type>(v));
  }

  constexpr auto &operator-=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = static_cast<value_type>(intermediate_value() - static_cast<storage_type>(v));
  }

  constexpr auto &operator*=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = value() * v;
  }

  constexpr auto &operator/=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = value() / v;
  }

  constexpr auto &operator%=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = value() % v;
  }

  constexpr auto &operator&=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = intermediate_value() & v;
  }

  constexpr auto &operator|=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = intermediate_value() | v;
  }

  constexpr auto &operator^=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = intermediate_value() ^ v;
  }

  constexpr auto &operator<<=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = intermediate_value() << v;
  }

  constexpr auto &operator>>=(value_type v) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = intermediate_value() >> v;
  }

  constexpr auto &operator++() noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = *this + 1;
  }

  constexpr value_type operator++(int) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    value_type tmp = value();
    ++*this;
    return tmp;
  }

  constexpr auto &operator--() noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    return *this = *this - 1;
  }

  constexpr value_type operator--(int) noexcept
    requires(std::integral<value_type> && std::assignable_from<storage_type &, value_type>)
  {
    value_type tmp = value();
    --*this;
    return tmp;
  }

private:
  constexpr auto intermediate_value() noexcept {
    return static_cast<std::remove_cv_t<storage_type>>(data >> mask.trailing_zeros());
  }
};

template <bitfield_compatible_type FieldType, std::unsigned_integral auto Mask>
constexpr auto as_bitfield(auto const &s) noexcept {
  return bitfield_view<std::remove_reference_t<decltype(s)>, bitfield_spec<FieldType, Mask>>(s);
}

template <is_bitfield_spec FieldSpec> constexpr auto as_bitfield(auto const &s) noexcept {
  return bitfield_view<std::remove_reference_t<decltype(s)>, FieldSpec>(s);
}

template <bitfield_compatible_type FieldType, std::unsigned_integral auto Mask>
constexpr auto as_writable_bitfield(auto &s) noexcept {
  return bitfield_view<std::remove_reference_t<decltype(s)>, bitfield_spec<FieldType, Mask>>(s);
}

template <is_bitfield_spec FieldSpec> constexpr auto as_writable_bitfield(auto &s) noexcept {
  return bitfield_view<std::remove_reference_t<decltype(s)>, FieldSpec>(s);
}

namespace bf_impl {
template <typename F, typename... Fields>
concept one_of = std::disjunction_v<std::is_same<F, Fields>...>;

template <std::uintmax_t... Masks>
concept non_overlaping = ((... & Masks) == 0);
} // namespace bf_impl

template <std::unsigned_integral StorageType, is_bitfield_spec... Fields>
  requires(bf_impl::non_overlaping<Fields::mask...> && fits_in<(... | Fields::mask), StorageType>)
class bitfield_set_view {
public:
  using field_types = std::tuple<Fields...>;
  using storage_type = StorageType;

  constexpr static bitmask<std::remove_cv_t<storage_type>> mask{
      static_cast<storage_type>((... | Fields::mask))};

  constexpr bitfield_set_view(StorageType &data) noexcept : m_data{data} {};

  template <typename F>
    requires(bf_impl::one_of<F, Fields...>)
  [[nodiscard]] constexpr auto get() noexcept {
    return as_writable_bitfield<F>(m_data);
  }

  template <typename F>
    requires(bf_impl::one_of<F, Fields...>)
  [[nodiscard]] constexpr auto get() const noexcept {
    return as_bitfield<F>(m_data);
  }

protected:
  StorageType &m_data;
};

template <is_bitfield_spec... Fields> constexpr auto as_bitfield_set(auto const &s) noexcept {
  return bitfield_set_view<std::remove_reference_t<decltype(s)>, Fields...>(s);
}

template <is_bitfield_spec... Fields> constexpr auto as_writable_bitfield_set(auto &s) noexcept {
  return bitfield_set_view<std::remove_reference_t<decltype(s)>, Fields...>(s);
}

} // namespace ecpp
#endif
