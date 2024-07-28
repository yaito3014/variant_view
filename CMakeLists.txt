cmake_minimum_required(VERSION 3.23)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING ""
  )
endif()

project(
  variant_view
  VERSION 0.1.0
  LANGUAGES CXX
)

add_library(variant_view INTERFACE)

add_library(yk::variant_view ALIAS variant_view)

file(GLOB_RECURSE VARIANT_VIEW_HEADERS CONFIGURE_DEPENDS include/*.hpp)
target_sources(variant_view INTERFACE FILE_SET HEADERS FILES ${VARIANT_VIEW_HEADERS})

include(GNUInstallDirs)
target_include_directories(
  variant_view INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
  variant_view
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
)

find_package(Boost COMPONENTS unit_test_framework CONFIG)
if(${Boost_FOUND})
  target_compile_definitions(variant_view INTERFACE YK_VARIANT_VIEW_INCLUDE_BOOST=1)
  target_link_libraries(variant_view INTERFACE Boost::headers)
endif()

install(
  TARGETS variant_view
  EXPORT variant_view-export
  FILE_SET HEADERS
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
  EXPORT variant_view-export
  FILE variant_viewTargets.cmake
  NAMESPACE yk::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)

if(${Boost_unit_test_framework_FOUND})
  add_subdirectory(test EXCLUDE_FROM_ALL)
else()
  message(STATUS "Boost.Test is not found, not building tests")
endif()
