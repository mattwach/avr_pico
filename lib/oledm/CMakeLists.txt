# This is only used with PI PICO and NOT AVR.
cmake_minimum_required(VERSION 3.12)

#project(TWI C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_library(OLEDM INTERFACE)
target_include_directories(OLEDM  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(OLEDM  INTERFACE)
target_sources(OLEDM  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/oledm.h
)

add_library(OLEDM_BITMAP INTERFACE)
target_link_libraries(OLEDM_BITMAP  INTERFACE OLEDM TWI)
target_sources(OLEDM_BITMAP  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/bitmap.c
)

add_library(OLEDM_CONSOLE INTERFACE)
target_link_libraries(OLEDM_CONSOLE  INTERFACE OLEDM TWI)
target_sources(OLEDM_CONSOLE  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/console.c
)

add_library(OLEDM_I2C INTERFACE)
target_link_libraries(OLEDM_I2C  INTERFACE OLEDM TWI)
target_sources(OLEDM_I2C  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/oledm_i2c.c
)

add_library(OLEDM_SPI INTERFACE)
target_link_libraries(OLEDM_SPI  INTERFACE OLEDM SPI)
target_sources(OLEDM_SPI  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/oledm_spi.c
)

add_library(OLEDM_SSD1306 INTERFACE)
target_link_libraries(OLEDM_SSD1306  INTERFACE OLEDM)
target_sources(OLEDM_SSD1306  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306.c
)

add_library(OLEDM_SH1106 INTERFACE)
target_link_libraries(OLEDM_SH1106  INTERFACE OLEDM)
target_sources(OLEDM_SH1106  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/sh1106.c
)

add_library(OLEDM_SH1107 INTERFACE)
target_link_libraries(OLEDM_SH1107  INTERFACE OLEDM)
target_sources(OLEDM_SH1107  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/sh1107.c
)

add_library(OLEDM_FIXED_TEXT INTERFACE)
target_link_libraries(OLEDM_FIXED_TEXT  INTERFACE OLEDM)
target_sources(OLEDM_FIXED_TEXT  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/fixed_text.c
)

add_library(OLEDM_TEXT INTERFACE)
target_link_libraries(OLEDM_TEXT  INTERFACE OLEDM)
target_sources(OLEDM_TEXT  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/text.c
)

add_subdirectory(font)
