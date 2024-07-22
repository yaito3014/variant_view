#ifndef YK_VARIANT_VIEW_DETAIL_VARIANT_VIEW_TRAITS_HPP
#define YK_VARIANT_VIEW_DETAIL_VARIANT_VIEW_TRAITS_HPP

#include <type_traits>

namespace yk {

template <class Variant, class... Ts>
class variant_view;

template <class Variant>
struct variant_dispatch;

template <class Variant>
struct is_variant_like : std::false_type {};

template <class Variant>
inline constexpr bool is_variant_like_v = is_variant_like<Variant>::value;

template <class Variant>
concept variant_like = is_variant_like_v<Variant>;

template <class Variant, class... Ts>
struct make_variant_view_result {
  using type = variant_view<Variant, Ts...>;
};

template <class Variant, class... Ts>
using make_variant_view_result_t = typename make_variant_view_result<Variant, Ts...>::type;

}  // namespace yk

namespace yk::detail {

template <class Variant, class VariantView, class T>
struct is_subtype_in_variant_view : std::false_type {};

template <class Variant, class VariantView, class T>
inline constexpr bool is_subtype_in_variant_view_v = is_subtype_in_variant_view<Variant, VariantView, T>::value;

template <class Variant, class VariantView, class... Ts>
struct is_subtypes_in_variant_view : std::conjunction<is_subtype_in_variant_view<Variant, VariantView, Ts>...> {};

template <class Variant, class VariantView, class... Ts>
inline constexpr bool is_subtypes_in_variant_view_v = is_subtypes_in_variant_view<Variant, VariantView, Ts...>::value;

}  // namespace yk::detail

#endif  // YK_VARIANT_VIEW_DETAIL_VARIANT_VIEW_TRAITS_HPP
