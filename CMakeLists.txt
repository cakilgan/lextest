cmake_minimum_required(VERSION 3.20)
project(lextest VERSION 1.0 LANGUAGES CXX)

add_library(lextest lextest.cpp)

target_include_directories(lextest PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(lextest PUBLIC cxx_std_17)
