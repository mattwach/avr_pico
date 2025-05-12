
ROOT=../../..
include $(ROOT)/vars.mak

CFLAGS := -DSOFTWARE_UART_BUFFER_SIZE=128


FILES := \
  main.o \
  $(ROOT_LIB)/uart/uart.o \
  $(ROOT_LIB)/uart/software_uart.o \
  $(ROOT_LIB)/pstr/pstr.o

include $(ROOT)/rules.mak

