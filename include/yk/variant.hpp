#ifndef YK_VARIANT_HPP
#define YK_VARIANT_HPP

#ifndef YK_VARIANT_VIEW_INCLUDE_STL
#define YK_VARIANT_VIEW_INCLUDE_STL 1
#endif

#if YK_VARIANT_VIEW_INCLUDE_STL
#include "yk/variant/std.hpp"
#endif

#if YK_VARIANT_VIEW_INCLUDE_BOOST
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

}  // namespace yk

#endif  // YK_VARIANT_HPP
