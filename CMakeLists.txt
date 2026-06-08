cmake_minimum_required(VERSION 3.12)
project(dedent
    VERSION 1.0.0
    DESCRIPTION "Compile-time string dedentation header-only library"
    HOMEPAGE_URL "https://github.com/0xA672/dedent.hpp"
    LANGUAGES CXX
)

add_library(dedent INTERFACE)
add_library(dedent::dedent ALIAS dedent)

target_include_directories(dedent INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(dedent INTERFACE cxx_std_17)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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

set(config_template "${CMAKE_CURRENT_BINARY_DIR}/dedent-config.cmake.in")
file(WRITE "${config_template}"
"@PACKAGE_INIT@

include(\"${CMAKE_CURRENT_LIST_DIR}/dedent-targets.cmake\")
")

configure_package_config_file(
    "${config_template}"
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
