#ifndef YK_VARIANT_VIEW_HPP
#define YK_VARIANT_VIEW_HPP

#ifndef YK_VARIANT_VIEW_INCLUDE_STL
#define YK_VARIANT_VIEW_INCLUDE_STL 1
#endif

#if YK_VARIANT_VIEW_INCLUDE_STL
#include "yk/variant_view/stl.hpp"
#endif

#if YK_VARIANT_VIEW_INCLUDE_BOOST
#include "yk/variant_view/boost.hpp"
#endif

#include "yk/variant/traits.hpp"

#include <type_traits>
#include <utility>

namespace yk {

namespace detail {

template <class... Ts>
struct variant_param_list {};

template <class T, class... Ts>
struct variant_param_list<T, Ts...> {
  using head = T;
};

}  // namespace detail

template <class Variant, class... Ts>
class variant_view {
public:
  static_assert(sizeof...(Ts) > 0, "empty variant_view is not allowed");
  static_assert(detail::VariantLike<std::remove_cvref_t<Variant>>, "argument should be variant-like type");
  static_assert((... && is_in_variant_v<std::remove_cvref_t<Variant>, Ts>), "variant_view must be subset of original variant");

  using variant_type = std::remove_const_t<Variant>;

  constexpr variant_view() noexcept : base_(nullptr) {}

  explicit constexpr variant_view(Variant& variant) noexcept : base_(&variant) {}

  explicit constexpr variant_view(variant_type&& variant) noexcept : base_(&variant) {}

  [[nodiscard]] const variant_type& base() const noexcept { return *base_; }
  [[nodiscard]] variant_type& base() noexcept
    requires(!std::is_const_v<Variant>)
  {
    return *base_;
  }

  template <class... Us>
    requires detail::is_subtypes_in_variant_view_v<std::remove_cvref_t<Variant>, variant_view<Variant, Us...>, Us...>
  [[nodiscard]] constexpr variant_view<const Variant, Us...> subview() const {
    return variant_view<const Variant, Us...>{*this};
  }

  template <class... Us>
    requires detail::is_subtypes_in_variant_view_v<Variant, variant_view<Variant, Us...>, Us...>
  [[nodiscard]] constexpr variant_view<Variant, Us...> subview() {
    return variant_view<Variant, Us...>{*this};
  }

private:
  template <class V, class... Us>
  friend class variant_view;

  template <class V, class... Us>
  explicit constexpr variant_view(variant_view<V, Us...>& view) : base_(view.base_) {}

  Variant* base_;
};

template <class... Ts, class Variant>
[[nodiscard]] constexpr auto make_variant_view(Variant&& variant) noexcept {
  return detail::make_variant_view_result_t<std::remove_reference_t<Variant>, Ts...>{std::forward<Variant>(variant)};
}

namespace detail {

template <class Variant>
struct visit_impl;

}

template <class Visitor, class Variant>
constexpr decltype(auto) visit(Visitor&& vis, Variant&& variant) {
  return detail::visit_impl<std::remove_cvref_t<Variant>>::apply(std::forward<Visitor>(vis), std::forward<Variant>(variant));
}

template <class Res, class Visitor, class Variant>
constexpr Res visit(Visitor&& vis, Variant&& variant) {
  return detail::visit_impl<std::remove_cvref_t<Variant>>::template apply<Res>(std::forward<Visitor>(vis), std::forward<Variant>(variant));
}

namespace detail {

template <class Visitor, class Variant, class... Ts>
struct SupersetTypeCatcher {
  using deduced_return_type = std::invoke_result_t<Visitor, typename detail::variant_param_list<Ts...>::head>;

  Visitor vis;
  template <class T>
  constexpr deduced_return_type operator()(T&& x) const {
    return std::invoke(vis, std::forward<T>(x));
  }

  template <class T>
    requires(!is_subtype_in_variant_view_v<std::remove_const_t<Variant>, variant_view<std::remove_const_t<Variant>, Ts...>, std::remove_cvref_t<T>>)
  [[noreturn]] constexpr deduced_return_type operator()(T&&) const {
    throw std::bad_variant_access{};
  }
};

template <class Variant, class... Ts>
struct visit_impl<variant_view<Variant, Ts...>> {
  template <class Visitor, class V>
  static constexpr decltype(auto) apply(Visitor&& vis, V&& view) {
    return yk::visit(SupersetTypeCatcher<Visitor, Variant, Ts...>{std::forward<Visitor>(vis)}, view.base());
  }

  template <class Res, class Visitor, class V>
  static constexpr Res apply(Visitor&& vis, V&& view) {
    return yk::visit<Res>(SupersetTypeCatcher<Visitor, Variant, Ts...>{std::forward<Visitor>(vis)}, view.base());
  }
};

}  // namespace detail

}  // namespace yk

#endif  // YK_VARIANT_VIEW_HPP
