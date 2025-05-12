# define to attiny85 or atmega328p
MCU?=attiny85

ROOT=../..
include $(ROOT)/vars.mak

FILES := \
  main.o \
  $(ROOT_LIB)/servo/servo_attiny.o \
  $(ROOT_LIB)/misc/debounce.o \

include $(ROOT)/rules.mak

