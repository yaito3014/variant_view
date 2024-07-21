cmake_minimum_required(VERSION 3.23)

find_package(Boost REQUIRED COMPONENTS unit_test_framework CONFIG)

add_executable(tests EXCLUDE_FROM_ALL test.cpp)
target_compile_features(tests PUBLIC cxx_std_20)
target_link_libraries(tests PUBLIC variant_view Boost::unit_test_framework)
add_test(NAME tests COMMAND tests)
