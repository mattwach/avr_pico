# define to attiny85 or atmega328p
MCU?=attiny85
IFACE?=i2c
OLED?=ssd1306
F_CPU=8000000

ROOT=../../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  hello_world_font.o \
  $(ROOT_LIB)/spi/spi.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/oledm/text.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \

include $(ROOT)/rules.mak

main.o: hello_world_font.h
		$(AVR_GCC) $(BASE_CFLAGS) $(CFLAGS) $(INCLUDES) -o $@ main.c

hello_world_font.h: hello_world_font.yaml
	$(ROOT_LIB)/oledm/tools/make_var_font.py hello_world_font.yaml

