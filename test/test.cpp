#define YK_VARIANT_VIEW_INCLUDE_STL 1
#define YK_VARIANT_VIEW_INCLUDE_BOOST 1
#include "yk/util/overloaded.hpp"
#include "yk/variant_view.hpp"

#define BOOST_TEST_MODULE yk_test
#include <boost/test/unit_test.hpp>

#include <boost/core/ignore_unused.hpp>
#include <boost/utility/identity_type.hpp>
#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <concepts>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(variant_view)

template <class... Ts>
using variant_t =
    std::tuple<std::variant<Ts...>, boost::variant<Ts...>, typename boost::make_recursive_variant<Ts..., std::vector<boost::recursive_variant_>>::type>;

#define YK_VARIANT(...) BOOST_IDENTITY_TYPE((variant_t<__VA_ARGS__>))

BOOST_AUTO_TEST_CASE_TEMPLATE(Initialization, Variant, YK_VARIANT(int, double)) {
  Variant v = 42;

  // identical sets (original variant set == view set)
  {
    auto view = yk::make_variant_view<int, double>(v);
    static_assert(std::same_as<decltype(view), yk::variant_view<Variant, int, double>>);
  }
  {
    auto view = yk::make_variant_view<int, double>(std::as_const(v));
    static_assert(std::same_as<decltype(view), yk::variant_view<const Variant, int, double>>);
  }
  {
    static_assert(std::is_constructible_v<yk::variant_view<Variant, int, double>, Variant&&>);
    auto view = yk::make_variant_view<int, double>(Variant{42});  // dangling view
    static_assert(std::same_as<decltype(view), yk::variant_view<Variant, int, double>>);
  }

  // clang-format off
  // direct initialization (variant_view view{variant};)
  {
    static_assert(std::is_nothrow_constructible_v<yk::variant_view<      Variant, int, double>, Variant>);
    static_assert(std::is_nothrow_constructible_v<yk::variant_view<const Variant, int, double>, Variant>);

    static_assert(std::is_nothrow_constructible_v<yk::variant_view<      Variant, int, double>, Variant&>);
    static_assert(std::is_nothrow_constructible_v<yk::variant_view<const Variant, int, double>, Variant&>);

    static_assert(!std::is_nothrow_constructible_v<yk::variant_view<     Variant, int, double>, const Variant&>);
    static_assert(std::is_nothrow_constructible_v<yk::variant_view<const Variant, int, double>, const Variant&>);

    static_assert(std::is_nothrow_constructible_v<yk::variant_view<      Variant, int, double>, Variant&&>);
    static_assert(std::is_nothrow_constructible_v<yk::variant_view<const Variant, int, double>, Variant&&>);
  }

  // trivial functions
  {
    static_assert(std::is_nothrow_default_constructible_v<yk::variant_view<      Variant, int, double>>);
    static_assert(std::is_nothrow_default_constructible_v<yk::variant_view<const Variant, int, double>>);

    static_assert(std::is_nothrow_copy_constructible_v<yk::variant_view<      Variant, int, double>>);
    static_assert(std::is_nothrow_copy_constructible_v<yk::variant_view<const Variant, int, double>>);

    static_assert(std::is_nothrow_move_constructible_v<yk::variant_view<      Variant, int, double>>);
    static_assert(std::is_nothrow_move_constructible_v<yk::variant_view<const Variant, int, double>>);

    static_assert(std::is_nothrow_copy_assignable_v<yk::variant_view<      Variant, int, double>>);
    static_assert(std::is_nothrow_copy_assignable_v<yk::variant_view<const Variant, int, double>>);

    static_assert(std::is_nothrow_move_assignable_v<yk::variant_view<      Variant, int, double>>);
    static_assert(std::is_nothrow_move_assignable_v<yk::variant_view<const Variant, int, double>>);

    static_assert(std::is_nothrow_convertible_v<yk::variant_view<       Variant, int, double>, yk::variant_view<const Variant, int, double>>);
    static_assert(!std::is_nothrow_convertible_v<yk::variant_view<const Variant, int, double>, yk::variant_view<      Variant, int, double>>);
  }
  // clang-format on

  // view copying
  {
    auto mutable_view = yk::make_variant_view<int, double>(v);

    static_assert(std::is_constructible_v<yk::variant_view<Variant, int, double>, decltype(mutable_view)>);
    static_assert(std::is_constructible_v<yk::variant_view<const Variant, int, double>, decltype(mutable_view)>);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SubView, Variant, YK_VARIANT(int, float, double)) {
  Variant v = 3.14;

  // trivial
  {
    // adding const
    [[maybe_unused]] yk::variant_view<const Variant, int, float> const_view = yk::make_variant_view(v).template subview<int, float>();
  }

  {
    // mutable version
    {
      auto view = yk::variant_view<Variant, int, float, double>(v);

      auto int_float_double_view = view.template subview<int, float, double>();
      static_assert(std::same_as<decltype(int_float_double_view), yk::variant_view<Variant, int, float, double>>);

      auto int_float_double_view2 = int_float_double_view.template subview<int, float, double>();
      static_assert(std::same_as<decltype(int_float_double_view2), decltype(int_float_double_view)>);

      auto int_float_view = view.template subview<int, float>();
      static_assert(std::same_as<decltype(int_float_view), yk::variant_view<Variant, int, float>>);

      auto int_view = int_float_view.template subview<int>();
      static_assert(std::same_as<decltype(int_view), yk::variant_view<Variant, int>>);
    }

    // const version
    {
      auto view = yk::variant_view<const Variant, int, float, double>(v);

      auto int_float_double_view = view.template subview<int, float, double>();
      static_assert(std::same_as<decltype(int_float_double_view), yk::variant_view<const Variant, int, float, double>>);

      auto int_float_double_view2 = int_float_double_view.template subview<int, float, double>();
      static_assert(std::same_as<decltype(int_float_double_view2), decltype(int_float_double_view)>);

      auto int_float_view = view.template subview<int, float>();
      static_assert(std::same_as<decltype(int_float_view), yk::variant_view<const Variant, int, float>>);

      auto int_view = int_float_view.template subview<int>();
      static_assert(std::same_as<decltype(int_view), yk::variant_view<const Variant, int>>);
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Visit, Variant, YK_VARIANT(int, double, std::string)) {
  // we are checking for potential implicit type conversion in this test...

  {
    const auto do_visit = [&](const auto& visitable) {
      return yk::visit(yk::overloaded{
                           [](const int&) -> std::string { return "int"; },
                           [](const double&) -> std::string { return "double"; },
                           [](const std::string&) -> std::string { return "string"; },
                           [](const std::vector<Variant>&) -> std::string { return "vector"; },
                       },
                       visitable);
    };

    BOOST_TEST(do_visit(Variant{42}) == "int");
    BOOST_TEST(do_visit(Variant{3.14}) == "double");
    BOOST_TEST(do_visit(Variant{std::string{"foo"}}) == "string");

    BOOST_TEST(do_visit(yk::variant_view<const Variant, int, double, std::string>{Variant{42}}) == "int");
    BOOST_TEST(do_visit(yk::variant_view<const Variant, int, double, std::string>{Variant{3.14}}) == "double");
    BOOST_TEST(do_visit(yk::variant_view<const Variant, int, double, std::string>{Variant{std::string{"foo"}}}) == "string");
  }
  {
    const auto do_visit_with_R = [&](const auto& visitable) {
      return yk::visit<std::string>(yk::overloaded{
                                        [](const int&) -> std::string { return "int"; },
                                        [](const double&) -> const char* { return "double"; },
                                        [](const std::string&) -> const char* { return "string"; },
                                        [](const std::vector<Variant>&) -> std::string { return "vector"; },
                                    },
                                    visitable);
    };

    BOOST_TEST(do_visit_with_R(Variant{42}) == "int");
    BOOST_TEST(do_visit_with_R(Variant{3.14}) == "double");
    BOOST_TEST(do_visit_with_R(Variant{std::string{"foo"}}) == "string");

    BOOST_TEST(do_visit_with_R(yk::variant_view<const Variant, int, double, std::string>{Variant{42}}) == "int");
    BOOST_TEST(do_visit_with_R(yk::variant_view<const Variant, int, double, std::string>{Variant{3.14}}) == "double");
    BOOST_TEST(do_visit_with_R(yk::variant_view<const Variant, int, double, std::string>{Variant{std::string{"foo"}}}) == "string");
  }

  // visiting subviews, with fully exhaustive visitor
  {
    const auto do_visit = [&](const auto& visitable) {
      return yk::visit(yk::overloaded{
                           [](const int&) -> std::string { return "int"; },
                           [](const double&) -> std::string { return "double"; },
                           [](const std::string&) -> std::string { return "string"; },
                       },
                       visitable);
    };

    BOOST_REQUIRE_THROW(boost::ignore_unused(do_visit(yk::make_variant_view(Variant{42}).template subview<double, std::string>()) == "int"),
                        std::bad_variant_access);

    BOOST_TEST(do_visit(yk::make_variant_view(Variant{3.14}).template subview<double, std::string>()) == "double");
    BOOST_TEST(do_visit(yk::make_variant_view(Variant{std::string{"foo"}}).template subview<double, std::string>()) == "string");
  }
}

BOOST_AUTO_TEST_SUITE_END()  // variant_view
