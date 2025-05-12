# This is only used with PI PICO and NOT AVR.
cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(DEBOUNCE INTERFACE)
target_include_directories(DEBOUNCE  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
target_sources(DEBOUNCE  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/debounce.h
    ${CMAKE_CURRENT_LIST_DIR}/debounce.c
)

add_library(GIMBAL INTERFACE)
target_include_directories(GIMBAL  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
target_sources(GIMBAL  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/gimbal.h
    ${CMAKE_CURRENT_LIST_DIR}/gimbal_state.h
    ${CMAKE_CURRENT_LIST_DIR}/gimbal.c
)

