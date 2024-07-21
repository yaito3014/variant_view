#ifndef YK_UTIL_SPECIALIZATION_OF_HPP
#define YK_UTIL_SPECIALIZATION_OF_HPP

#include <type_traits>

namespace yk {

template <template <class...> class TT, class T>
struct is_specialization_of : std::false_type {};

template <template <class...> class TT, class... Ts>
struct is_specialization_of<TT, TT<Ts...>> : std::true_type {};

template <template <class...> class TT, class T>
inline constexpr bool is_specialization_of_v = is_specialization_of<TT, T>::value;

template <class T, template <class...> class TT>
concept specialization_of = is_specialization_of_v<TT, T>;

}  // namespace yk

#endif  // YK_UTIL_SPECIALIZATION_OF_HPP
