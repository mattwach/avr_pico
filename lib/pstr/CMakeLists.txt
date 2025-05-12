# This is only used with PI PICO and NOT AVR.
cmake_minimum_required(VERSION 3.12)

#project(TWI C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_library(PSTR INTERFACE)
target_include_directories(PSTR  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
target_sources(PSTR  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pstr.h
    ${CMAKE_CURRENT_LIST_DIR}/pstr.c
)

