cmake_minimum_required(VERSION 3.1)
project(!PROJECT! LANGUAGES !LANGUAGE!)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_!LANGUAGE!_VERSION !STANDARD!)

include(static_analyzers)

#
# find_package(package CONFIG REQUIRED)
# . . .
#
# set(!PROJECT!_ExternalLibraries)
#

add_subdirectory(!PROJECT!)
