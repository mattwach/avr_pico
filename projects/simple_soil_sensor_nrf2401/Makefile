# define to attiny85 or atmega328p
MCU?=atmega328p
IFACE?=i2c
OLED?=ssd1306

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/rf24/rf24.o \
  $(ROOT_LIB)/spi/spi.o \

include $(ROOT)/rules.mak

