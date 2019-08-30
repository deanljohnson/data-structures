cmake_minimum_required(VERSION 3.13)
project(data-structures)

set (CMAKE_CXX_STANDARD 17)

set(DOC_TEST_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/doctest")

add_subdirectory(src)