# define to attiny85 or atmega328p
MCU?=atmega328p

ROOT=../../../..

include $(ROOT)/vars.mak

CFLAGS=-DOLEDM_COLOR16 -DOLEDM_EMULATE_MONOCHROME

FILES := \
  main.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/oledm/ssd1780.o \
  $(ROOT_LIB)/oledm/oledm_spi.o \

include $(ROOT)/rules.mak

