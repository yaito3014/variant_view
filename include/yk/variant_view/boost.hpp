#ifndef YK_VARIANT_VIEW_BOOST_HPP
#define YK_VARIANT_VIEW_BOOST_HPP

#include "yk/util/all_same.hpp"
#include "yk/util/exactly_once.hpp"
#include "yk/util/find_type_index.hpp"
#include "yk/variant/traits.hpp"
#include "yk/variant_view/detail/variant_view_traits.hpp"

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/variant.hpp>
#include <type_traits>
#include <utility>

namespace yk {

template <class... Ts, class T>
struct is_in_variant<boost::variant<Ts...>, T> : std::disjunction<std::is_same<Ts, T>...> {};

namespace detail {

template <class... Ts, class... Us, class T>
struct is_subtype_in_variant_view<boost::variant<Ts...>, variant_view<Us...>, T> : std::disjunction<std::is_same<Us, T>...> {
  static_assert((... || std::is_same_v<Ts, T>), "T must be in variant's template parameters");
};

template <class... Ts>
struct variant_like<boost::variant<Ts...>> : std::true_type {};

template <class... Ts>
struct make_variant_view_result<boost::variant<Ts...>> {
  using type = variant_view<boost::variant<Ts...>, Ts...>;
};

template <class Variant>
struct visit_impl;

template <class... Ts>
struct visit_impl<boost::variant<Ts...>> {
private:
  template <class Visitor, class Res>
  struct Wrapper {
    using result_type = Res;
    Visitor vis;
    template <class T>
    constexpr Res operator()(T&& x) const {
      return std::invoke(vis, std::forward<T>(x));
    }
  };

public:
  template <class Visitor, class Variant>
  static constexpr decltype(auto) apply(Visitor&& vis, Variant&& variant) {
    static_assert(core::is_all_same_v<std::invoke_result_t<Visitor, Ts>...>, "visitor must return same type for all possible parameters");
    return boost::apply_visitor(std::forward<Visitor>(vis), std::forward<Variant>(variant));
  }

  template <class Res, class Visitor, class Variant>
  static constexpr Res apply(Visitor&& vis, Variant&& variant) {
    Wrapper<Visitor, Res> wrapper{std::forward<Visitor>(vis)};
    return std::forward<Variant>(variant).apply_visitor(wrapper);
  }
};

}  // namespace detail

template <class T, class... Ts>
[[nodiscard]] /* constexpr */ bool holds_alternative(const boost::variant<Ts...>& v) noexcept {
  static_assert(core::exactly_once_v<T, Ts...>);
  return core::find_type_index_v<T, Ts...> == v.which();
}

template <class T, class... Ts, class... Us>
[[nodiscard]] /* constexpr */ bool holds_alternative(const variant_view<boost::variant<Ts...>, Us...>& v) noexcept {
  static_assert(core::exactly_once_v<T, Ts...>);
  return core::find_type_index_v<T, Ts...> == v.base().which();
}

}  // namespace yk

#endif  // YK_VARIANT_VIEW_BOOST_HPP
