file(GLOB EXEC_SOURCES *.cpp)

set(ALL_EXECUTABLES "")

foreach(SRC_FILE ${EXEC_SOURCES})
    get_filename_component(EXEC_NAME ${SRC_FILE} NAME_WE)
    add_executable(${EXEC_NAME} ${SRC_FILE})
    set_project_warnings(${EXEC_NAME})
    target_link_libraries(${EXEC_NAME} PRIVATE wiringPi srcLib color_lib)
    list(APPEND ALL_EXECUTABLES ${EXEC_NAME})
endforeach()

message ("${ALL_EXECUTABLES}")
install(TARGETS ${ALL_EXECUTABLES} DESTINATION bin)
