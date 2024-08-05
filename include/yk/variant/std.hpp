#ifndef YK_VARIANT_STD_HPP
#define YK_VARIANT_STD_HPP

#include "yk/variant/traits.hpp"

#include <type_traits>
#include <variant>

namespace yk {

template <class... Ts, class T>
struct is_in_variant<std::variant<Ts...>, T> : std::disjunction<std::is_same<Ts, T>...> {};

template <class... Ts>
struct is_variant_like<std::variant<Ts...>> : std::true_type {};

template <class... Ts>
struct variant_dispatch<std::variant<Ts...>> {
  template <class Visitor, class Variant>
  static constexpr decltype(auto) apply_visit(Visitor&& vis, Variant&& variant) {
    return std::visit(std::forward<Visitor>(vis), std::forward<Variant>(variant));
  }

  template <class Res, class Visitor, class Variant>
  static constexpr Res apply_visit(Visitor&& vis, Variant&& variant) {
    return std::visit<Res>(std::forward<Visitor>(vis), std::forward<Variant>(variant));
  }

  static constexpr std::size_t apply_index(const std::variant<Ts...>& var) noexcept { return var.index(); }
};

template <class T, class... Ts>
[[nodiscard]] constexpr bool holds_alternative(const std::variant<Ts...>& v) noexcept {
  return std::holds_alternative<T>(v);
}

template <class T, class StdVariant>
  requires specialization_of<std::remove_cvref_t<StdVariant>, std::variant>
[[nodiscard]] constexpr decltype(auto) get(StdVariant&& variant) {
  return std::get<T>(std::forward<StdVariant>(variant));
}

template <std::size_t I, class StdVariant>
  requires specialization_of<std::remove_cvref_t<StdVariant>, std::variant>
[[nodiscard]] constexpr decltype(auto) get(StdVariant&& variant) {
  return std::get<I>(std::forward<StdVariant>(variant));
}

template <class T, class StdVariant>
  requires specialization_of<std::remove_const_t<StdVariant>, std::variant>
[[nodiscard]] constexpr auto get(StdVariant* variant) noexcept {
  return std::get_if<T>(variant);
}

template <std::size_t I, class StdVariant>
  requires specialization_of<std::remove_const_t<StdVariant>, std::variant>
[[nodiscard]] constexpr auto get(StdVariant* variant) noexcept {
  return std::get_if<I>(variant);
}

}  // namespace yk

#endif  // YK_VARIANT_STD_HPP
