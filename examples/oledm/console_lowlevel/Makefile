# define to attiny85 or atmega328p
MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

ROOT=../../..

include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/oledm/font/terminus8x16.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/twi/twi.o \

include $(ROOT)/rules.mak

