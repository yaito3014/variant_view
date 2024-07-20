cmake_minimum_required(VERSION 3.23)

add_executable(tests test.cpp)
target_compile_features(tests PUBLIC cxx_std_20)
target_link_libraries(tests PUBLIC variant_view)
add_test(NAME tests COMMAND tests)
