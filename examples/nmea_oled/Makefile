include ../../vars.mak
MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

CFLAGS := -DNMEA_BUFFER_SIZE=1024

FILES := \
  main.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/nmea_decoder/nmea_decoder.o \
  $(ROOT_LIB)/oledm/$(OLED).o \
  $(ROOT_LIB)/oledm/font/terminus6x8.o \
  $(ROOT_LIB)/oledm/font/terminus16x32_numbers.o \
  $(ROOT_LIB)/oledm/oledm_$(IFACE).o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/oledm/fixed_text.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/uart/uart.o

include ../../rules.mak

