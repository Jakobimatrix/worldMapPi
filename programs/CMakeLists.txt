cmake_minimum_required(VERSION 3.16)

project(
  WorldMap
  VERSION 0.0.1
  DESCRIPTION "To dominate the world one day"
  LANGUAGES CXX C)

# Detect compiler and set flags
include(cmake/DetectCompiler.cmake)
include(cmake/CompilerSetup.cmake)
include(cmake/Warnings.cmake)

# Find WiringPi
find_library(WIRINGPI_LIB wiringPi REQUIRED)

set(BUILD_TESTS OFF)
set(TESTS_SETTINGS OFF)

# Add subdirectories
add_subdirectory(src)

