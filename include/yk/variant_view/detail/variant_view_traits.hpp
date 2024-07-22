#ifndef YK_VARIANT_VIEW_DETAIL_VARIANT_VIEW_TRAITS_HPP
#define YK_VARIANT_VIEW_DETAIL_VARIANT_VIEW_TRAITS_HPP

#include <type_traits>

namespace yk {

template <class Variant, class... Ts>
class variant_view;

template <class Variant, class... Ts>
struct make_variant_view_result {
  using type = variant_view<Variant, Ts...>;
};

template <class Variant, class... Ts>
using make_variant_view_result_t = typename make_variant_view_result<Variant, Ts...>::type;

template <class VariantView, class T>
struct is_in_variant_view : std::false_type {};

template <class VariantView, class T>
inline constexpr bool is_in_variant_view_v = is_in_variant_view<VariantView, T>::value;

namespace detail {

template <class VariantView, class... Ts>
struct is_subtypes_in_variant_view : std::conjunction<is_in_variant_view<VariantView, Ts>...> {};

template <class VariantView, class... Ts>
inline constexpr bool is_subtypes_in_variant_view_v = is_subtypes_in_variant_view<VariantView, Ts...>::value;

}  // namespace detail

}  // namespace yk

#endif  // YK_VARIANT_VIEW_DETAIL_VARIANT_VIEW_TRAITS_HPP
