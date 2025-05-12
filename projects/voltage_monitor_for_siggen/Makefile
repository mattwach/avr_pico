# define to attiny85 or atmega328p
MCU?=attiny85

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/oledm/font/terminus16x32_numbers.o \
  $(ROOT_LIB)/oledm/oledm_i2c.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/ssd1306.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/twi/twi.o \

include $(ROOT)/rules.mak

