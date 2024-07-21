cmake_minimum_required(VERSION 3.23)

find_package(Boost REQUIRED COMPONENTS unit_test_framework CONFIG)

add_executable(test test.cpp)
target_compile_features(test PUBLIC cxx_std_20)
target_link_libraries(test PUBLIC variant_view Boost::unit_test_framework)
add_test(NAME test COMMAND test)
