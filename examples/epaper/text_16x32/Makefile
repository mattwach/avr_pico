# define to attiny85 or atmega328p
MCU?=atmega328p

CFLAGS := -DSSD1680

ROOT=../../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/oledm/ssd1680.o \
  $(ROOT_LIB)/oledm/oledm_spi.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/oledm/font/terminus16x32.o \

include $(ROOT)/rules.mak

