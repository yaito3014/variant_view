#ifndef YK_VARIANT_VIEW_BOOST_HPP
#define YK_VARIANT_VIEW_BOOST_HPP

#include "yk/util/all_same.hpp"
#include "yk/util/exactly_once.hpp"
#include "yk/util/find_type_index.hpp"
#include "yk/util/pack_indexing.hpp"
#include "yk/variant/traits.hpp"
#include "yk/variant_view/detail/variant_view_traits.hpp"

#include <boost/mpl/contains.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace yk {

template <class... Ts, class T>
struct is_in_variant<boost::variant<Ts...>, T> : std::bool_constant<boost::mpl::contains<typename boost::variant<Ts...>::types, T>::type::value> {};

namespace detail {

template <class... Ts>
struct type_list {};

template <class T, class R>
struct to_type_list;

template <class... Ts, class X>
struct to_type_list<type_list<Ts...>, X> {
  using type = type_list<Ts..., X>;
};

template <class BoostVariant>
using boost_variant_types_t = typename boost::mpl::fold<typename BoostVariant::types, type_list<>, to_type_list<boost::mpl::_1, boost::mpl::_2>>::type;

template <class... Ts, class... Us, class T>
struct is_subtype_in_variant_view<boost::variant<Ts...>, variant_view<boost::variant<Ts...>, Us...>, T> : std::disjunction<std::is_same<Us, T>...> {
  static_assert(boost::mpl::contains<typename boost::variant<Ts...>::types, T>::type::value, "T must be in variant's template parameters");
};

template <class... Ts>
struct variant_like<boost::variant<Ts...>> : std::true_type {};

template <class... Ts>
struct make_variant_view_result<boost::variant<Ts...>> {
  template <class TypeList>
  struct helper {};

  template <class... Us>
  struct helper<type_list<Us...>> {
    using type = variant_view<boost::variant<Ts...>, Us...>;
  };
  using type = typename helper<boost_variant_types_t<boost::variant<Ts...>>>::type;
};

}  // namespace detail

template <class... Ts>
struct variant_dispatch<boost::variant<Ts...>> {
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
  static constexpr decltype(auto) apply_visit(Visitor&& vis, Variant&& variant) {
    []<class... Us>(detail::type_list<Us...>) {
      static_assert(core::is_all_same_v<std::invoke_result_t<Visitor, Us>...>, "visitor must return same type for all possible parameters");
    }(detail::boost_variant_types_t<std::remove_cvref_t<Variant>>{});
    return boost::apply_visitor(std::forward<Visitor>(vis), std::forward<Variant>(variant));
  }

  template <class Res, class Visitor, class Variant>
  static constexpr Res apply_visit(Visitor&& vis, Variant&& variant) {
    Wrapper<Visitor, Res> wrapper{std::forward<Visitor>(vis)};
    return std::forward<Variant>(variant).apply_visitor(wrapper);
  }

  static constexpr std::size_t apply_index(const boost::variant<Ts...>& var) noexcept { return static_cast<std::size_t>(var.which()); }
};

template <class T, class... Ts>
[[nodiscard]] /* constexpr */ bool holds_alternative(const boost::variant<Ts...>& v) noexcept {
  return [&]<class... Us>(detail::type_list<Us...>) {
    static_assert(core::exactly_once_v<T, Us...>);
    return core::find_type_index_v<T, Us...> == v.which();
  }(detail::boost_variant_types_t<boost::variant<Ts...>>{});
}

template <class T, class... Ts, class... Us>
[[nodiscard]] /* constexpr */ bool holds_alternative(const variant_view<boost::variant<Ts...>, Us...>& v) noexcept {
  return [&]<class... Vs>(detail::type_list<Vs...>) {
    static_assert(core::exactly_once_v<T, Vs...>);
    return core::find_type_index_v<T, Vs...> == v.base().which();
  }(detail::boost_variant_types_t<boost::variant<Ts...>>{});
}

template <class T, class... Ts, class... Us>
[[nodiscard]] /* constexpr */ bool holds_alternative(const variant_view<const boost::variant<Ts...>, Us...>& v) noexcept {
  return [&]<class... Vs>(detail::type_list<Vs...>) {
    static_assert(core::exactly_once_v<T, Vs...>);
    return core::find_type_index_v<T, Vs...> == v.base().which();
  }(detail::boost_variant_types_t<boost::variant<Ts...>>{});
}

template <class T, class BoostVariant>
  requires specialization_of<std::remove_cvref_t<BoostVariant>, boost::variant>
[[nodiscard]] constexpr decltype(auto) get(BoostVariant&& variant) try {
  return boost::get<T>(std::forward<BoostVariant>(variant));
} catch (const boost::bad_get&) {
  throw std::bad_variant_access{};
}

template <std::size_t I, class BoostVariant>
  requires specialization_of<std::remove_cvref_t<BoostVariant>, boost::variant>
[[nodiscard]] constexpr decltype(auto) get(BoostVariant&& variant) try {
  return [&]<class... Vs>(detail::type_list<Vs...>) -> decltype(auto) {
    return boost::get<pack_indexing_t<I, Vs...>>(std::forward<BoostVariant>(variant));
  }(detail::boost_variant_types_t<std::remove_cvref_t<BoostVariant>>{});
} catch (const boost::bad_get&) {
  throw std::bad_variant_access{};
}

template <class T, class BoostVariant>
  requires specialization_of<std::remove_const_t<BoostVariant>, boost::variant>
[[nodiscard]] constexpr auto get(BoostVariant* variant) noexcept {
  return boost::get<T>(variant);
}

template <std::size_t I, class BoostVariant>
  requires specialization_of<std::remove_const_t<BoostVariant>, boost::variant>
[[nodiscard]] constexpr auto get(BoostVariant* variant) noexcept {
  return [&]<class... Vs>(detail::type_list<Vs...>) {
    return boost::get<pack_indexing_t<I, Vs...>>(variant);
  }(detail::boost_variant_types_t<std::remove_cvref_t<BoostVariant>>{});
}

// template <class... Ts>
// variant_view(const boost::variant<Ts...>&) -> /* impossible due to boost::recursive_variant_ */;

// template <class... Ts>
// variant_view(boost::variant<Ts...>&) -> /* impossible due to boost::recursive_variant_ */;

}  // namespace yk

#endif  // YK_VARIANT_VIEW_BOOST_HPP
