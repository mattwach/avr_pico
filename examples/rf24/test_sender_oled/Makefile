MCU?=atmega328p

ROOT=../../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/oledm/console.o \
  $(ROOT_LIB)/oledm/font/terminus6x8.o \
  $(ROOT_LIB)/oledm/ssd1306.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/oledm/oledm_i2c.o \
  $(ROOT_LIB)/rf24/rf24.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/twi/twi.o \

include $(ROOT)/rules.mak

