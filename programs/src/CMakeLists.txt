# Find all .cpp files
file(GLOB SRC_FILES *.cpp)

add_library(srcLib INTERFACE 
    ${SRC_FILES}
)

# Ensure the current source directory is included for header-only files or .cpp files
target_include_directories(srcLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


# color lib
add_library(color_lib INTERFACE)
target_include_directories(color_lib INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/color/include
)
