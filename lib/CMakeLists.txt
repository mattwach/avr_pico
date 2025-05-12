# This is ONLY used for PI PICO and NOT AVR
cmake_minimum_required(VERSION 3.12)

add_library(AVRLIB INTERFACE)
target_include_directories(AVRLIB  INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(misc)
add_subdirectory(pstr)
add_subdirectory(spi)
add_subdirectory(twi)
add_subdirectory(oledm)

