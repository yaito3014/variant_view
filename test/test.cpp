#include <boost/variant.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <variant>

#define YK_VARIANT_VIEW_INCLUDE_BOOST 1
#include "yk/util/overloaded.hpp"
#include "yk/variant_view.hpp"

struct A {};
struct B {};
struct C {};

int main() {  //
  std::cout << std::boolalpha;
  {
    [[maybe_unused]] std::variant<A, B, C> var = A{};
    [[maybe_unused]] auto view = yk::make_variant_view<A, B>(var);

    // [[maybe_unused]] auto subv = view.subview<A>();
    yk::visit(yk::overloaded{[](auto x) { std::puts("num num"); }}, view);

    std::cout << yk::holds_alternative<A>(var) << std::endl;
    std::cout << yk::holds_alternative<A>(view) << std::endl;
  }

  {
    [[maybe_unused]] boost::variant<A, B, C> var = A{};
    [[maybe_unused]] auto view = yk::make_variant_view<A, B>(var);
    [[maybe_unused]] auto subv = view.subview<A>();

    yk::visit([](A x) { std::puts("A only"); }, subv);

    std::cout << yk::holds_alternative<A>(var) << std::endl;
    std::cout << yk::holds_alternative<A>(view) << std::endl;
  }

  {
    std::variant<int, float> var = 42;
    auto view = yk::make_variant_view<int, float>(var);
    std::cout << yk::holds_alternative<int>(view) << std::endl;

    yk::visit(yk::overloaded{
                  [](int x) { std::puts("int"); },
                  [](float x) { std::puts("float"); },
              },
              var);

    std::cout << yk::visit<int>(yk::overloaded{
                                    [](int x) -> short { return 33; },
                                    [](float x) -> float { return 4; },
                                },
                                var)
              << std::endl;
  }

  {
    boost::variant<int, float> var = 42;
    auto view = yk::make_variant_view<int, float>(var);
    std::cout << yk::holds_alternative<int>(view) << std::endl;

    yk::visit(yk::overloaded{
                  [](int x) { std::puts("int"); },
                  [](float x) { std::puts("float"); },
              },
              var);

    std::cout <<  //
        yk::visit<int>(yk::overloaded{
                           [](int x) -> short { return 33; },
                           [](float x) -> float { return 3.14f; },
                       },
                       var)
              << std::endl;
  }
}
