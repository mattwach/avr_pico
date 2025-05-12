# define to attint84, attiny85 or atmega328p
MCU?=attiny84
IFACE?=i2c
OLED?=ssd1306
F_CPU=8000000

ROOT=../../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/time/measure.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/oledm/font/terminus8x16.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \

include $(ROOT)/rules.mak

