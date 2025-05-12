include ../../vars.mak

CFLAGS := -DDEBUG -DTWI_FREQ=100000

FILES := \
  main.o \
  $(ROOT_LIB)/weather/ms8607.o \
  $(ROOT_LIB)/twi/twi.o \
  $(ROOT_LIB)/uart/uart.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/lowpower/lowpower.o

include ../../rules.mak

