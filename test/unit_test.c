#include "unit_test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global state
const uint16_t* checked_register[65536];  // A queue of registers that were checked in test cases
unsigned int checked_register_idx;

extern void avr_reset(void);

// Checks all registers that were accessed against all that
// are known.  Errors is  register changes but was never checked
// in the test.
// 
// The point to to avoid unintended hardware changes slipping through
// the test process
static void check_register_access() {
  // Simple O(n^2) algorithm which should be fine for the small data set size
  int i=0;
  uint8_t fail = 0;
  for (; i < KNOWN_REGISTER_COUNT; ++i) {
    if (*KNOWN_REGISTER_INDEX[i] > 0) {
      // This register was accessed
      int j=0;
      for (; j < checked_register_idx; ++j) {
        if (checked_register[j] == KNOWN_REGISTER_INDEX[i]) {
          break;
        }
      }
      if (j == checked_register_idx) {
        fail = 1;
        printf("\n  %s was changed in hardware but not asserted in the testcase.  ",
            KNOWN_REGISTER_NAME[i]);
      }
    }
  }

  if (fail) {
    exit(1);
  }
}

void test_(void (*fn)(void), const char* test_name) {
  checked_register_idx = 0;
  avr_reset();
  printf("Running %s: ", test_name);
  fn();
  check_register_access();
  printf("OK\n");
}

#define HEX_LINE_LENGTH 64
static void dump_hex_line(const uint8_t* data, unsigned int length, const uint8_t* compare_data) {
  int i=0;
  int pos = 0;
  for (; i<length; ++i) {
    putc(
        compare_data && (compare_data[i] != data[i]) ?
        '>' :
        ' ',
    stdout);
    ++pos;
    pos += printf("%02X", data[i]);
    if (((i + 1) % 8) == 0) {
      putc(' ', stdout);
      ++pos;
      if (((i + 1) % 4) == 0) {
        putc(' ', stdout);
        ++pos;
      }
    }
  }
  
  for (i=pos; i<HEX_LINE_LENGTH; ++i) {
    putc(' ', stdout);
  }
}

static void dump_ascii(const uint8_t* data, unsigned int length) {
  int i=0;
  for (; i < length; ++i) {
    char c = (data[i] >= ' ' && data[i] <= 126) ? (char)data[i] : '.';
    putc(c, stdout);
  }
}

static void dump_bytes(const uint8_t* data, unsigned int length, const uint8_t* compare_data) {
  unsigned int i = 0;
  for (; i < length; i += 16) {
    printf("\n   ");
    unsigned int bytes_to_dump = length - i;
    if (bytes_to_dump > 16) {
      bytes_to_dump = 16;
    }
    dump_hex_line(data + i, bytes_to_dump,
        compare_data ? compare_data + i : 0);
    printf(" | ");
    dump_ascii(data + i, bytes_to_dump);
  }
  putc('\n', stdout);
}

static void dump_u32(const uint32_t* data, unsigned int length, const uint32_t* compare_data) {
  unsigned int i = 0;
  for (; i < length; ++i) {
    if (data[i] == compare_data[i]) {
      printf("%4u: %08X          OK  [%u]\n", i, data[i], data[i]);
    } else {
      printf("%4u: %08X != %08X  [%u != %u]\n",
          i, data[i], compare_data[i], data[i], compare_data[i]);
    }
  }
  putc('\n', stdout);
}

static void ignore_reg(const uint16_t* index_reg) {
  checked_register[checked_register_idx++] = index_reg;
}

void assert_reg_activity_(const uint16_t* index_reg, const char* reg_name) {
  ignore_reg(index_reg);
  if (*index_reg == 0) {
    printf("\n  %s(%s): Registor was never accessed.\n",
           __FUNCTION__, reg_name);
    exit(1);
  }
}

void assert_history_(
  const uint8_t* expected_history,
  const uint8_t* actual_history,
  const uint8_t current_value,
  uint8_t expected_index,
  const uint16_t* index_reg,
  const char* reg_name) {

  ignore_reg(index_reg);
  const uint8_t actual_index = *index_reg;

  // need to create a new history that ignores the first byte of 
  // history and adds the current calue at the end
  uint8_t history[256];
  if (actual_index < 1) {
    printf("\n  %s(%s): Registor was never accessed.\n",
           __FUNCTION__, reg_name);
    exit(1);
  }
  if (actual_index > 1) {
    memcpy(history, actual_history + 1, actual_index - 1);
  }
  history[actual_index - 1] = current_value;

  if ((expected_index != actual_index) || memcmp(expected_history, history, expected_index)) {
    printf("\n  %s(%s):\n", __FUNCTION__, reg_name);
    printf("  expected");
    dump_bytes(expected_history, expected_index, history);
    printf("  got");
    dump_bytes(history, actual_index, expected_history);
    exit(1);
  }
}


void reset_history(void) {
  int i=0;
  for (; i < KNOWN_REGISTER_COUNT; ++i) {
    *KNOWN_REGISTER_INDEX[i] = 0;
  }
}

void assert_reg_equal_(
  int expected,
  int actual,
  const uint16_t* index_reg,
  const char* reg_name) {
  ignore_reg(index_reg);
  if (*index_reg == 0) {
    printf("\n  %s(%s): Registor was never accessed.\n",
           __FUNCTION__, reg_name);
    exit(1);
  }
  if (expected != actual) {
    printf("\n  %s(%s): expected %d (0x%02X), got %d (0x%02X)\n",
           __FUNCTION__, reg_name, expected, expected, actual, actual);
    exit(1);
  }
}

void assert_buff_equal_(
  const uint8_t* expected,
  const uint8_t* actual,
  const unsigned int length,
  const char* expected_name,
  const char* actual_name) {

  if (expected == NULL && actual != NULL) {
    printf("\n  %s(%s, %s):\n", __FUNCTION__, expected_name, actual_name);
    printf("  expected (NULL).  got");
    dump_bytes(actual, length, NULL);
    exit(1);
  } else if (expected != NULL && actual == NULL) {
    printf("\n  %s(%s, %s):\n", __FUNCTION__, expected_name, actual_name);
    printf("  expected");
    dump_bytes(expected, length, NULL);
    printf("  got (NULL)\n");
    exit(1);
  } else if (memcmp(expected, actual, length)) {
    printf("\n  %s(%s, %s):\n", __FUNCTION__, expected_name, actual_name);
    printf("  expected");
    dump_bytes(expected, length, actual);
    printf("  got");
    dump_bytes(actual, length, expected);
    exit(1);
  }
}

void assert_u32_array_equal_(
  const uint32_t* expected,
  const uint32_t* actual,
  const unsigned int length,
  const char* expected_name,
  const char* actual_name) {

  if (expected == NULL && actual != NULL) {
    printf("\n  %s(%s, %s):\n", __FUNCTION__, expected_name, actual_name);
    printf("  expected (NULL).  got");
    dump_u32(actual, length, NULL);
    exit(1);
  } else if (expected != NULL && actual == NULL) {
    printf("\n  %s(%s, %s):\n", __FUNCTION__, expected_name, actual_name);
    printf("  expected");
    dump_u32(expected, length, NULL);
    printf("  got (NULL)\n");
    exit(1);
  } else if (memcmp(expected, actual, length * sizeof(uint32_t))) {
    printf("\n  %s(%s, %s):\n", __FUNCTION__, expected_name, actual_name);
    dump_u32(expected, length, actual);
    exit(1);
  }
}


void assert_ptr_equal_(
  void* expected,
  void* actual,
  const char* expected_name,
  const char* actual_name) {
  if (expected != actual) {
    printf("\n  %s(%s, %s): expected %p, got %p\n",
           __FUNCTION__, expected_name, actual_name, expected, actual);
    exit(1);
  }
}

void assert_int_equal_(
  int expected,
  int actual,
  const char* expected_name,
  const char* actual_name) {
  if (expected != actual) {
    printf("\n  %s(%s, %s): expected %d (0x%02X), got %d (0x%02X)\n",
           __FUNCTION__, expected_name, actual_name, expected, expected,
           actual, actual);
    exit(1);
  }
}

void assert_float_near_equal_(
  float expected,
  float actual,
  const char* expected_name,
  const char* actual_name) {
  float difference = expected - actual;
  if (difference < 0) {
    difference = -difference;
  }
  if (difference > 0.000001) {
    printf("\n  %s(%s, %s): expected %f, got %f\n",
           __FUNCTION__, expected_name, actual_name, expected,
           actual);
    exit(1);
  }
}

void assert_double_near_equal_(
  double expected,
  double actual,
  const char* expected_name,
  const char* actual_name) {
  double difference = expected - actual;
  if (difference < 0) {
    difference = -difference;
  }
  if (difference > 0.000000001) {
    printf("\n  %s(%s, %s): expected %.9f, got %.9f\n",
           __FUNCTION__, expected_name, actual_name, expected,
           actual);
    exit(1);
  }
}

void assert_int_gt_(
  int expected,
  int actual,
  const char* expected_name,
  const char* actual_name) {
  if (expected <= actual) {
    printf("\n  %s(%s, %s): expected %d (0x%02X) > %d (0x%02X)\n",
           __FUNCTION__, expected_name, actual_name, expected, expected, actual, actual);
    exit(1);
  }
}

void assert_int_lt_(
  int expected,
  int actual,
  const char* expected_name,
  const char* actual_name) {
  if (expected >= actual) {
    printf("\n  %s(%s, %s): expected %d (0x%02X) < %d (0x%02X)\n",
           __FUNCTION__, expected_name, actual_name, expected, expected, actual, actual);
    exit(1);
  }
}

void sequence_(
    uint8_t* sequence_reg,
    uint16_t* sequence_max,
    uint16_t* reg_index,
    uint8_t value) {
  sequence_reg[*sequence_max] = value;
  ++(*sequence_max);
  ignore_reg(reg_index);
}
