cmake_minimum_required(VERSION 3.20)
project(lextest-examples VERSION 0.1 LANGUAGES CXX)
include(../cmake/lextest.cmake)

lx_add_test(example-1 test1.cpp)
