#ifndef YK_VARIANT_TRAITS_HPP
#define YK_VARIANT_TRAITS_HPP

#include <type_traits>

namespace yk {

template <class Variant, class T>
struct is_in_variant : std::false_type {};

template <class Variant, class T>
inline constexpr bool is_in_variant_v = is_in_variant<Variant, T>::value;

}  // namespace yk

#endif  // YK_VARIANT_TRAITS_HPP
