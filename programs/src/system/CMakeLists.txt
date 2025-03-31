add_library(system_lib STATIC
  src/system/system.cpp
  src/system/system.hpp
  )


target_include_directories(system_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
