#ifndef YK_VARIANT_VIEW_STL_HPP
#define YK_VARIANT_VIEW_STL_HPP

#include "yk/variant/traits.hpp"
#include "yk/variant_view/detail/variant_view_traits.hpp"

#include <type_traits>
#include <utility>
#include <variant>

namespace yk {

namespace detail {

template <class... Ts, class... Us, class T>
struct is_subtype_in_variant_view<std::variant<Ts...>, variant_view<std::variant<Ts...>, Us...>, T> : std::disjunction<std::is_same<Us, T>...> {
  static_assert((... || std::is_same_v<Ts, T>), "T must be in variant's template parameters");
};

template <class... Ts>
struct variant_like<std::variant<Ts...>> : std::true_type {};

template <class... Ts>
struct make_variant_view_result<std::variant<Ts...>, Ts...> {
  using type = variant_view<std::variant<Ts...>, Ts...>;
};

template <class... Ts>
struct make_variant_view_result<std::variant<Ts...>> {
  using type = variant_view<std::variant<Ts...>, Ts...>;
};

template <class Variant>
struct visit_impl;

template <class... Ts>
struct visit_impl<std::variant<Ts...>> {
  template <class Visitor, class Variant>
  static constexpr decltype(auto) apply(Visitor&& vis, Variant&& variant) {
    return std::visit(std::forward<Visitor>(vis), std::forward<Variant>(variant));
  }

  template <class Res, class Visitor, class Variant>
  static constexpr Res apply(Visitor&& vis, Variant&& variant) {
    return std::visit<Res>(std::forward<Visitor>(vis), std::forward<Variant>(variant));
  }
};

}  // namespace detail

template <class... Ts, class T>
struct is_in_variant<std::variant<Ts...>, T> : std::disjunction<std::is_same<Ts, T>...> {};

template <class T, class... Ts>
[[nodiscard]] constexpr bool holds_alternative(const std::variant<Ts...>& v) noexcept {
  return std::holds_alternative<T>(v);
}

template <class T, class... Ts, class... Us>
[[nodiscard]] constexpr bool holds_alternative(const variant_view<std::variant<Ts...>, Us...>& v) noexcept {
  return std::holds_alternative<T>(v.base());
}

template <class T, class... Ts, class... Us>
[[nodiscard]] constexpr bool holds_alternative(const variant_view<const std::variant<Ts...>, Us...>& v) noexcept {
  return std::holds_alternative<T>(v.base());
}

template <class... Ts>
variant_view(const std::variant<Ts...>&) -> variant_view<const std::variant<Ts...>, Ts...>;

template <class... Ts>
variant_view(std::variant<Ts...>&) -> variant_view<std::variant<Ts...>, Ts...>;

}  // namespace yk

#endif  // YK_VARIANT_VIEW_STL_HPP
