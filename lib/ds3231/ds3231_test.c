#include "ds3231.h"
#include <twi/twi_fake.h>

#include <test/unit_test.h>

// Directly include some deps to avoid making the test makefile more complex
#include <twi/twi_fake.c>

void test_basic_init(void) {
  struct DS3231 ds3231;
  ds3231_init(&ds3231);
  assert_int_equal(0, ds3231.error);
}

void test_read_and_parse(void) {
  struct DS3231 ds3231;
  ds3231_init(&ds3231);
  twi_log_reset();

  struct DS3231 ds3231_expected;
  ds3231_set_seconds(&ds3231_expected, 21);
  ds3231_set_minutes(&ds3231_expected, 32);
  ds3231_set_hours_24(&ds3231_expected, 23);
  ds3231_set_day_of_week(&ds3231_expected, 4);
  ds3231_set_day_of_month(&ds3231_expected, 18);
  ds3231_set_month(&ds3231_expected, 11);
  ds3231_set_year(&ds3231_expected, 56);

  twi_set_read_data(ds3231_expected.data);
  ds3231_read(&ds3231);

  assert_buff_equal(
      ((uint8_t[]){
       TWI_START_WRITE, 0x68,
       TWI_WRITE_NO_STOP, 0x00,
       TWI_READ_NO_STOP, 0x68, 0x07,
       TWI_STOP,
       }),
      twi_log,
      twi_logidx
    );

  assert_int_equal(21, ds3231_parse_seconds(&ds3231));
  assert_int_equal(32, ds3231_parse_minutes(&ds3231));
  assert_int_equal(23, ds3231_parse_hours_24(&ds3231));
  assert_int_equal(4, ds3231_parse_day_of_week(&ds3231));
  assert_int_equal(18, ds3231_parse_day_of_month(&ds3231));
  assert_int_equal(11, ds3231_parse_month(&ds3231));
  assert_int_equal(56, ds3231_parse_year(&ds3231));
}

void test_write(void) {
  struct DS3231 ds3231;
  ds3231_init(&ds3231);
  twi_log_reset();

  for (uint8_t i = 0; i < DS3231_READ_LENGTH; ++i) {
    ds3231.data[i] = i + 0x30;
  }

  ds3231_write(&ds3231);

  assert_buff_equal(
      ((uint8_t[]){
       TWI_START_WRITE, 0x68,
       TWI_WRITE_NO_STOP, 0x00,
       TWI_WRITE_NO_STOP, 0x30,
       TWI_WRITE_NO_STOP, 0x31,
       TWI_WRITE_NO_STOP, 0x32,
       TWI_WRITE_NO_STOP, 0x33,
       TWI_WRITE_NO_STOP, 0x34,
       TWI_WRITE_NO_STOP, 0x35,
       TWI_WRITE_NO_STOP, 0x36,
       TWI_STOP,
       }),
      twi_log,
      twi_logidx
    );

}

int main(void) {
    test(test_basic_init);
    test(test_read_and_parse);
    test(test_write);
    return 0;
}
