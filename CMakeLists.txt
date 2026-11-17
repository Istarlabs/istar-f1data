# SPDX-FileCopyrightText: 2026 Pascal Klose
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)

project(istar-f1data VERSION 0.1.0 LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # At least as strict as the applications that add the library, so a warning shows up here first. An application
    # brings its own flags; these are not forced on it.
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

find_package(Qt6 6.8 REQUIRED COMPONENTS Core)

add_library(istar-f1data STATIC
    src/f1data/names.cpp
    src/f1data/names.h
    src/f1data/packets.h
    src/f1data/parser.cpp
    src/f1data/parser.h
)
add_library(IstarF1Data::F1Data ALIAS istar-f1data)

# std::expected
target_compile_features(istar-f1data PUBLIC cxx_std_23)
target_include_directories(istar-f1data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(istar-f1data PUBLIC Qt6::Core)
# Linked into a shared library or a position-independent executable as well.
set_target_properties(istar-f1data PROPERTIES POSITION_INDEPENDENT_CODE ON)

# The tests come with the library on its own, not when an application adds it with add_subdirectory().
option(ISTAR_F1DATA_TESTS "Build the tests of istar-f1data" ${PROJECT_IS_TOP_LEVEL})

if(ISTAR_F1DATA_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
