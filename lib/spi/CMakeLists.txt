# This is only used with PI PICO and NOT AVR.
cmake_minimum_required(VERSION 3.12)

#project(TWI C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_library(SPI INTERFACE)
target_include_directories(SPI  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(SPI  INTERFACE pico_stdlib hardware_spi)
target_sources(SPI  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/spi.h
    ${CMAKE_CURRENT_LIST_DIR}/spi_pi_pico.c
)

