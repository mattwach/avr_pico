# define to attiny85 or atmega328p
MCU?=attiny85

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  large_font.o \
  calibration_font.o \
  $(ROOT_LIB)/adc/adc.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/oledm/oledm_i2c.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/ssd1306.o \
  $(ROOT_LIB)/oledm/text.o \
  $(ROOT_LIB)/twi/twi.o \

include $(ROOT)/rules.mak

main.o: large_font.h calibration_font.h
	$(AVR_GCC) $(BASE_CFLAGS) $(CFLAGS) $(INCLUDES) -o $@ main.c

large_font.h: large_font.yaml
	$(ROOT_LIB)/oledm/tools/make_var_font.py large_font.yaml

calibration_font.h: calibration_font.yaml
	$(ROOT_LIB)/oledm/tools/make_var_font.py calibration_font.yaml

