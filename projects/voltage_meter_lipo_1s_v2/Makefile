# define to attiny85 or atmega328p
MCU?=attiny85
F_CPU=8000000

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  number_font.o \
  $(ROOT_LIB)/adc/adc.o \
  $(ROOT_LIB)/oledm/oledm_i2c.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/ssd1306.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/oledm/text.o \

include $(ROOT)/rules.mak

number_font.h: number_font.yaml
	$(ROOT_LIB)/oledm/tools/make_var_font.py number_font.yaml
