#ifndef YK_VARIANT_HPP
#define YK_VARIANT_HPP

#ifdef YK_VARIANT_INCLUDE_STL
#error "did you mean YK_VARIANT_INCLUDE_STD?"
#endif

#ifndef YK_VARIANT_INCLUDE_STD
#define YK_VARIANT_INCLUDE_STD 1
#endif

#if YK_VARIANT_INCLUDE_STD
#include "yk/variant/std.hpp"
#endif

#if YK_VARIANT_INCLUDE_BOOST
#include "yk/variant/boost.hpp"
#endif

namespace yk {

template <class Visitor, class Variant>
constexpr decltype(auto) visit(Visitor&& vis, Variant&& variant) {
  return variant_dispatch<std::remove_cvref_t<Variant>>::apply_visit(std::forward<Visitor>(vis), std::forward<Variant>(variant));
}

template <class Res, class Visitor, class Variant>
constexpr Res visit(Visitor&& vis, Variant&& variant) {
  return variant_dispatch<std::remove_cvref_t<Variant>>::template apply_visit<Res>(std::forward<Visitor>(vis), std::forward<Variant>(variant));
}

namespace detail {

template <class Func, class... Ts>
struct BindBack {
  Func f;
  std::tuple<Ts...> args;

  template <class... Us>
  constexpr decltype(auto) operator()(Us&&... xs) const {
    return std::apply(
        [&](auto&&... ys) -> decltype(auto) { return std::invoke(std::forward<Func>(f), std::forward<Us>(xs)..., std::forward<decltype(ys)>(ys)...); }, args);
  }

  template <class... Us>
  constexpr decltype(auto) operator()(Us&&... xs) {
    return std::apply(
        [&](auto&&... ys) -> decltype(auto) { return std::invoke(std::forward<Func>(f), std::forward<Us>(xs)..., std::forward<decltype(ys)>(ys)...); }, args);
  }
};

template <class Visitor, class Variant>
struct BoundVisitor {
  Visitor visitor;
  Variant variant;

  template <class T>
  constexpr decltype(auto) operator()(T&& x) const {
    return visit(BindBack<Visitor, T>(visitor, {std::forward<T>(x)}), variant);
  }

  template <class T>
  constexpr decltype(auto) operator()(T&& x) {
    return visit(BindBack<Visitor, T>(std::forward<Visitor>(visitor), {std::forward<T>(x)}), variant);
  }
};

template <class Ret, class Visitor, class Variant>
struct BoundVisitorWithRet {
  Visitor visitor;
  Variant variant;

  template <class T>
  constexpr decltype(auto) operator()(T&& x) const {
    return visit<Ret>(BindBack<Visitor, T>(visitor, {std::forward<T>(x)}), variant);
  }

  template <class T>
  constexpr decltype(auto) operator()(T&& x) {
    return visit<Ret>(BindBack<Visitor, T>(std::forward<Visitor>(visitor), {std::forward<T>(x)}), variant);
  }
};

}  // namespace detail

template <class Visitor, class Variant1, class Variant2, class... Variants>
constexpr decltype(auto) visit(Visitor&& vis, Variant1&& variant1, Variant2&& variant2, Variants&&... variants) {
  return visit(detail::BoundVisitor<Visitor, Variant1>{std::forward<Visitor>(vis), std::forward<Variant1>(variant1)}, std::forward<Variant2>(variant2),
               std::forward<Variants>(variants)...);
}

template <class Res, class Visitor, class Variant1, class Variant2, class... Variants>
constexpr Res visit(Visitor&& vis, Variant1&& variant1, Variant2&& variant2, Variants&&... variants) {
  return visit<Res>(detail::BoundVisitorWithRet<Res, Visitor, Variant1>{std::forward<Visitor>(vis), std::forward<Variant1>(variant1)},
                    std::forward<Variant2>(variant2), std::forward<Variants>(variants)...);
}

}  // namespace yk

#endif  // YK_VARIANT_HPP
