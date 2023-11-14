cmake_minimum_required(VERSION 3.1)
project(cppman LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_VERSION 23)

include(static_analyzers)

#
# find_package(package CONFIG REQUIRED)
# . . .
#
# set(cppman_ExternalLibraries)
#

add_subdirectory(cppman)
