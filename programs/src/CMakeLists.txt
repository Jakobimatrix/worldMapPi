# Find all .cpp files
file(GLOB SRC_FILES *.cpp)

# If there are no .cpp files, we just skip creating the static library
if(SRC_FILES)
    add_library(srcLib STATIC ${SRC_FILES})
else()
    # For header-only libraries, we simply create a target without any source files
    add_library(srcLib INTERFACE)
endif()

# Ensure the current source directory is included for header-only files or .cpp files
target_include_directories(srcLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR/lib}
)


# color lib
add_library(color_lib INTERFACE)
target_include_directories(color_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/color/include")
