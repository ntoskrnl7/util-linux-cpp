cmake_minimum_required(VERSION 3.10)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(pkg-utils)
include(CPM)

pkg_get_version(ULNX ${CMAKE_CURRENT_SOURCE_DIR}/include/util_linux/_version VERSION)

project(util-linux VERSION "${VERSION}" LANGUAGES C CXX)

option(ULNX_BUILD_TESTS "Set to ON to build tests" OFF)
option(ULNX_NO_NLOHMANN_JSON "Set to ON to exclude the nlohmann json library" OFF)
option(ULNX_NO_EXT "Set to ON to exclude the ext library" OFF)

add_library(util-linux INTERFACE)

target_include_directories(util-linux INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (NOT ULNX_NO_NLOHMANN_JSON)
    CPMAddPackage("gh:nlohmann/json@3.10.5")
endif()

if (NOT ULNX_NO_EXT)
    CPMAddPackage("gh:ntoskrnl7/ext@0.5.6")
endif()

target_link_libraries(util-linux INTERFACE ext nlohmann_json stdc++fs)

if (ULNX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()