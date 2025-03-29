file(GLOB EXEC_SOURCES *.cpp)

foreach(SRC_FILE ${EXEC_SOURCES})
    get_filename_component(EXEC_NAME ${SRC_FILE} NAME_WE)
    add_executable(${EXEC_NAME} ${SRC_FILE})
    target_link_libraries(${EXEC_NAME} PRIVATE wiringPi srcLib)
    list(APPEND ALL_EXECUTABLES ${EXEC_NAME})
endforeach()
