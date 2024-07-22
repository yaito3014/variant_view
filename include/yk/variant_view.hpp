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

#include "yk/util/pack_indexing.hpp"
#include "yk/variant/traits.hpp"

#include <compare>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace yk {

namespace detail {

template <class Variant>
struct visit_impl;

template <class Variant>
struct variant_index_impl;

}  // namespace detail

template <class Variant, class... Ts>
class variant_view {
public:
  static_assert(sizeof...(Ts) > 0, "empty variant_view is not allowed");
  static_assert(detail::VariantLike<std::remove_cvref_t<Variant>>, "argument should be variant-like type");
  static_assert((... && is_in_variant_v<std::remove_cvref_t<Variant>, Ts>), "variant_view must be subset of original variant");

  using variant_type = std::remove_const_t<Variant>;

  constexpr variant_view() noexcept = default;

  constexpr variant_view(const variant_view&) noexcept = default;
  constexpr variant_view(variant_view&&) noexcept = default;
  constexpr variant_view& operator=(const variant_view&) noexcept = default;
  constexpr variant_view& operator=(variant_view&&) noexcept = default;

  explicit constexpr variant_view(Variant& variant) noexcept : base_(&variant) {}

  explicit constexpr variant_view(variant_type&& variant) noexcept : base_(&variant) {}

  template <class V, class... Us>
    requires(std::is_const_v<Variant> || !std::is_const_v<V>)
  constexpr variant_view(const variant_view<V, Us...>& other) noexcept : base_(other.base_) {
    static_assert(detail::is_subtypes_in_variant_view_v<std::remove_const_t<Variant>, variant_view<std::remove_const_t<Variant>, Us...>, Us...>,
                  "only operations which take subset is allowed");
  }

  template <class... Us>
  constexpr variant_view& operator=(const variant_view<std::remove_const_t<Variant>, Us...>& other) noexcept {
    base_ = other.base_;
    return *this;
  }

  [[nodiscard]] constexpr const variant_type& base() const noexcept { return *base_; }
  [[nodiscard]] constexpr variant_type& base() const noexcept
    requires(!std::is_const_v<Variant>)
  {
    return *base_;
  }
  [[nodiscard]] constexpr variant_type& base() noexcept
    requires(!std::is_const_v<Variant>)
  {
    return *base_;
  }

  [[nodiscard]] constexpr decltype(auto) operator*() const
    requires(sizeof...(Ts) == 1);

  [[nodiscard]] constexpr decltype(auto) operator*()
    requires(sizeof...(Ts) == 1);

  [[nodiscard]] constexpr auto operator->() const noexcept
    requires(sizeof...(Ts) == 1);

  [[nodiscard]] constexpr auto operator->() noexcept
    requires(sizeof...(Ts) == 1);

  [[nodiscard]] explicit constexpr operator bool() const noexcept;

  template <class... Us>
  [[nodiscard]] constexpr variant_view<const Variant, Us...> subview() const noexcept {
    return variant_view<const Variant, Us...>{*this};
  }

  template <class... Us>
  [[nodiscard]] constexpr variant_view<Variant, Us...> subview() noexcept {
    return variant_view<Variant, Us...>{*this};
  }

  template <class Visitor>
  constexpr decltype(auto) visit(Visitor&& vis) const;

  template <class Res, class Visitor>
  constexpr Res visit(Visitor&& vis) const;

  template <class Visitor>
  constexpr decltype(auto) visit(Visitor&& vis);

  template <class Res, class Visitor>
  constexpr Res visit(Visitor&& vis);

  [[nodiscard]] constexpr std::size_t index() const noexcept { return detail::variant_index_impl<std::remove_const_t<Variant>>::apply(base()); }
  [[nodiscard]] constexpr bool invalid() const noexcept { return base_ == nullptr; }

  [[nodiscard]] constexpr bool operator==(const variant_view& other) const noexcept { return base() == other.base(); }
  [[nodiscard]] constexpr auto operator<=>(const variant_view& other) const noexcept { return base() <=> other.base(); }

  constexpr void swap(variant_view& other) noexcept { std::swap(base_, other.base_); }

private:
  template <class V, class... Us>
  friend class variant_view;

  Variant* base_;
};

template <class... Ts, class Variant>
[[nodiscard]] constexpr auto make_variant_view(Variant&& variant) noexcept {
  return detail::make_variant_view_result_t<std::remove_reference_t<Variant>, Ts...>{std::forward<Variant>(variant)};
}

template <class Visitor, class Variant>
constexpr decltype(auto) visit(Visitor&& vis, Variant&& variant) {
  return detail::visit_impl<std::remove_cvref_t<Variant>>::apply(std::forward<Visitor>(vis), std::forward<Variant>(variant));
}

template <class Res, class Visitor, class Variant>
constexpr Res visit(Visitor&& vis, Variant&& variant) {
  return detail::visit_impl<std::remove_cvref_t<Variant>>::template apply<Res>(std::forward<Visitor>(vis), std::forward<Variant>(variant));
}

template <class Variant, class... Ts>
template <class Visitor>
constexpr decltype(auto) variant_view<Variant, Ts...>::visit(Visitor&& vis) const {
  return yk::visit(std::forward<Visitor>(vis), *this);
}

template <class Variant, class... Ts>
template <class Res, class Visitor>
constexpr Res variant_view<Variant, Ts...>::visit(Visitor&& vis) const {
  return yk::visit<Res>(std::forward<Visitor>(vis), *this);
}

template <class Variant, class... Ts>
template <class Visitor>
constexpr decltype(auto) variant_view<Variant, Ts...>::visit(Visitor&& vis) {
  return yk::visit(std::forward<Visitor>(vis), *this);
}

template <class Variant, class... Ts>
template <class Res, class Visitor>
constexpr Res variant_view<Variant, Ts...>::visit(Visitor&& vis) {
  return yk::visit<Res>(std::forward<Visitor>(vis), *this);
}

template <class T, class VariantView>
  requires specialization_of<std::remove_cvref_t<VariantView>, variant_view>
[[nodiscard]] constexpr decltype(auto) get(VariantView&& view) {
  return yk::get<T>(std::forward<VariantView>(view).base());
}

template <std::size_t I, class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) get(const variant_view<Variant, Ts...>& view) {
  return yk::get<pack_indexing_t<I, Ts...>>(view.base());
}
template <std::size_t I, class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) get(variant_view<Variant, Ts...>& view) {
  return yk::get<pack_indexing_t<I, Ts...>>(view.base());
}
template <std::size_t I, class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) get(variant_view<Variant, Ts...>&& view) {
  return yk::get<pack_indexing_t<I, Ts...>>(std::move(view).base());
}
template <std::size_t I, class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) get(const variant_view<Variant, Ts...>&& view) {
  return yk::get<pack_indexing_t<I, Ts...>>(std::move(view).base());
}

template <class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) variant_view<Variant, Ts...>::operator*() const
  requires(sizeof...(Ts) == 1)
{
  return yk::get<0>(*this);
}

template <class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) variant_view<Variant, Ts...>::operator*()
  requires(sizeof...(Ts) == 1)
{
  return yk::get<0>(*this);
}

template <class T, class VariantView>
  requires specialization_of<std::remove_const_t<VariantView>, variant_view>
[[nodiscard]] constexpr auto get(VariantView* view) {
  return yk::get<T>(&view->base());
}

template <std::size_t I, class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) get(const variant_view<Variant, Ts...>* view) {
  return yk::get<pack_indexing_t<I, Ts...>>(&view->base());
}
template <std::size_t I, class Variant, class... Ts>
[[nodiscard]] constexpr decltype(auto) get(variant_view<Variant, Ts...>* view) {
  return yk::get<pack_indexing_t<I, Ts...>>(&view->base());
}

template <class Variant, class... Ts>
[[nodiscard]] constexpr auto variant_view<Variant, Ts...>::operator->() const noexcept
  requires(sizeof...(Ts) == 1)
{
  return yk::get<0>(this);
}

template <class Variant, class... Ts>
[[nodiscard]] constexpr auto variant_view<Variant, Ts...>::operator->() noexcept
  requires(sizeof...(Ts) == 1)
{
  return yk::get<0>(this);
}

template <class Variant, class... Ts>
[[nodiscard]] constexpr variant_view<Variant, Ts...>::operator bool() const noexcept {
  return yk::get<0>(this) != nullptr;
}

template <class Variant, class... Ts>
constexpr void swap(variant_view<Variant, Ts...>& x, variant_view<Variant, Ts...>& y) noexcept {
  x.swap(y);
}

namespace detail {

template <class Visitor, class Variant, class... Ts>
struct SupersetTypeCatcher {
  using deduced_return_type = std::invoke_result_t<Visitor, pack_indexing_t<0, Ts...>>;

  Visitor vis;
  template <class T>
  constexpr deduced_return_type operator()(T&& x) const {
    return std::invoke(vis, std::forward<T>(x));
  }

  template <class T>
  constexpr deduced_return_type operator()(T&& x) {
    return std::invoke(vis, std::forward<T>(x));
  }

  template <class T>
    requires(!is_subtype_in_variant_view_v<std::remove_const_t<Variant>, variant_view<std::remove_const_t<Variant>, Ts...>, std::remove_cvref_t<T>>)
  [[noreturn]] constexpr deduced_return_type operator()(T&&) const {
    throw std::bad_variant_access{};
  }

  template <class T>
    requires(!is_subtype_in_variant_view_v<std::remove_const_t<Variant>, variant_view<std::remove_const_t<Variant>, Ts...>, std::remove_cvref_t<T>>)
  [[noreturn]] constexpr deduced_return_type operator()(T&&) {
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
