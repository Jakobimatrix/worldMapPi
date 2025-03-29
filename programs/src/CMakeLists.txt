file(GLOB SRC_FILES *.cpp)
add_library(srcLib STATIC ${SRC_FILES})
target_include_directories(srcLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
