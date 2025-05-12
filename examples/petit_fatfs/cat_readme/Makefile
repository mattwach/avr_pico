MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

ROOT=../../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/oledm/console.o \
  $(ROOT_LIB)/oledm/font/terminus6x8.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/petit_fatfs/avr_mmcp.o \
  $(ROOT_LIB)/petit_fatfs/pff.o \

include $(ROOT)/rules.mak

