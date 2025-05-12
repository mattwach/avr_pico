MCU?=atmega328p
# Using the pro mini 3.3V @ 8mhz
F_CPU=8000000

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/rf24/rf24.o \
  $(ROOT_LIB)/spi/spi.o \

include $(ROOT)/rules.mak

