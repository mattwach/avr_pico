INCLUDES:= \
  -I../.. \
  -I../../lib \
  -I../../test/include \

F_CPU?=16000000

BASE_CFLAGS:= \
  -c \
  -g \
  -std=c11 \
  -DUNIT_TEST \
  -Wall \
  -Werror \
  -DF_CPU=$(F_CPU) \

SHELL=/bin/bash -o pipefail

SUPPORT_OBJECTS += \
  ../../test/include/avr/io.o \
  ../../test/unit_test.o

MODULE_TEST_LIST:=$(subst .c,,$(wildcard *_test.c))
HEADER_ONLY_MODULE_TEST_LIST:=$(subst .c,,$(wildcard *_test_h_only.c))

test_all: clean mk_io $(MODULE_TEST_LIST) $(HEADER_ONLY_MODULE_TEST_LIST)

mk_io:
	cd ../../test/include/avr && python3 mk_io.py

$(MODULE_TEST_LIST): %_test: %.o %_test.o $(SUPPORT_OBJECTS) 
	gcc -o $@ $^
	./$@ | tee last_test_run.txt && python3 ../../test/check_test_results.py last_test_run.txt $@.o

$(HEADER_ONLY_MODULE_TEST_LIST): %_test_h_only: %_test_h_only.o $(SUPPORT_OBJECTS) 
	gcc -o $@ $^
	./$@ | tee last_test_run.txt && python3 ../../test/check_test_results.py last_test_run.txt $@.o

%.o: %.c
	gcc $(BASE_CFLAGS) $(CFLAGS) $(INCLUDES) -o $@ $?

clean:
	rm -f *.o $(MODULE_TEST_LIST) $(HEADER_ONLY_MODULE_TEST_LIST) $(SUPPORT_OBJECTS) last_test_run.txt

