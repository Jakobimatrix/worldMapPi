set(ALL_EXECUTABLES "")


add_executable(statusLED src/statusLED.cpp)
target_link_libraries(statusLED PRIVATE wiringPi systemLib)
list(APPEND ALL_EXECUTABLES statusLED)

add_executable(shutdownButton src/shutdownButton.cpp)
target_link_libraries(shutdownButton PRIVATE wiringPi systemLib)
list(APPEND ALL_EXECUTABLES shutdownButton)

add_executable(LEDstripe src/LEDstripe.cpp)
target_link_libraries(LEDstripe PRIVATE wiringPi systemLib color_lib)
list(APPEND ALL_EXECUTABLES LEDstripe)



message ("${ALL_EXECUTABLES}")
install(TARGETS ${ALL_EXECUTABLES} DESTINATION bin)
