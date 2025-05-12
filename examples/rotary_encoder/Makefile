# define to attiny85 or atmega328p
MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/misc/debounce.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/time/measure_no_interrupt_atmega328p_timer0.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/oledm/font/terminus16x32_upper.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \

include $(ROOT)/rules.mak

