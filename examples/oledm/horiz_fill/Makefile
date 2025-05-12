# define to attiny85 or atmega328p
MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

ROOT=../../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \

include $(ROOT)/rules.mak

