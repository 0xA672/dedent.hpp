cmake_minimum_required(VERSION 3.12)
project(dedent
    VERSION 1.1.0
    DESCRIPTION "Compile-time string dedentation header-only library"
    HOMEPAGE_URL "https://github.com/0xA672/dedent.hpp"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(dedent INTERFACE)
add_library(dedent::dedent ALIAS dedent)

target_include_directories(dedent INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(dedent INTERFACE cxx_std_17)

install(TARGETS dedent EXPORT dedent-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(EXPORT dedent-targets
    NAMESPACE dedent::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dedent"
)
install(FILES "dedent.hpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/dedent-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dedent-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dedent"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dedent-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dedent-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dedent-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dedent"
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  enable_testing()
  add_executable(dedent_tests tests/dedent_tests.cpp)
  target_link_libraries(dedent_tests PRIVATE dedent::dedent)
  add_test(NAME dedent_tests COMMAND dedent_tests)
endif()
