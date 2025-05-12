include ../../vars.mak
MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

F_CPU=8000000

FILES := \
  main.o \
  $(ROOT_LIB)/ina260/ina260.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/console.o \
  $(ROOT_LIB)/oledm/font/terminus6x8.o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/uart/uart.o

include ../../rules.mak

