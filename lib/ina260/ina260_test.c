#include "ina260.h"
#include <twi/twi_fake.h>

#include <test/unit_test.h>

// Directly include some deps to avoid making the test makefile more complex
#include <twi/twi_fake.c>

void test_basic_init(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);

    assert_int_equal(0x40, ina260.i2c_address);
    assert_int_equal(0, ina260.error);
}

void test_read_configuration(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();

    uint16_t expected_config = ina260_create_configuration(
        AVG_128_SAMPLES,
        CONVERSION_TIME_204_US,
        CONVERSION_TIME_1100_US,
        OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE);
    uint8_t read_buffer[] = {(expected_config >> 8), expected_config & 0xFF};
    twi_set_read_data(read_buffer);

    uint16_t configuration = ina260_read_configuration(&ina260);
    assert_int_equal(
        AVG_128_SAMPLES, ina260_parse_averaging_mode(configuration));
    assert_int_equal(
        CONVERSION_TIME_204_US,
        ina260_parse_voltage_conversion_time(configuration));
    assert_int_equal(
        CONVERSION_TIME_1100_US,
        ina260_parse_current_conversion_time(configuration));
    assert_int_equal(
        CONVERSION_TIME_1100_US,
        ina260_parse_current_conversion_time(configuration));
    assert_int_equal(
        OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE,
        ina260_parse_operating_mode(configuration));

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_READ_NO_STOP, 0x40, 0x02,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_read_configuration_with_error(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();
    twi_queue_err(0x23);

    uint16_t configuration = ina260_read_configuration(&ina260);
    assert_int_equal(0, configuration);
    assert_int_equal(0x23, ina260.error);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
        }),
        twi_log,
        twi_logidx
    );
}

void test_create_configuration(void) {
  uint16_t config = ina260_create_configuration(
      AVG_512_SAMPLES,
      CONVERSION_TIME_4156_US,
      CONVERSION_TIME_588_US,
      OPERATING_MODE_CONTINUOUS_CURRENT);
  // 0110 1101 1001 1101
  assert_int_equal(0x6D9D, config);
}

void test_write_configuration(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();

    ina260_write_configuration(&ina260, 0x6123);

    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x61,
            TWI_WRITE_NO_STOP, 0x23,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_set_operating_mode(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();

    uint16_t expected_config = ina260_create_configuration(
        AVG_128_SAMPLES,
        CONVERSION_TIME_204_US,
        CONVERSION_TIME_1100_US,
        OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE);
    uint8_t read_buffer[] = {(expected_config >> 8), expected_config & 0xFF};
    twi_set_read_data(read_buffer);

    ina260_set_operating_mode(&ina260, OPERATING_MODE_CONTINUOUS_CURRENT);
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_READ_NO_STOP, 0x40, 0x02,
            TWI_STOP,
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x00,
            TWI_WRITE_NO_STOP, 0x68,
            TWI_WRITE_NO_STOP, 0x65,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_read_current_in_ma(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();

    const uint16_t current_value = 1000;
    uint8_t read_buffer[] = {(current_value >> 8), current_value & 0xFF};
    twi_set_read_data(read_buffer);

    // 1000 * 1.25 + 2 = 1252.  2 is the default current_bias_ma;
    assert_int_equal(1252, ina260_read_current_in_ma(&ina260));
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x01,
            TWI_READ_NO_STOP, 0x40, 0x02,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_read_voltage_in_mv(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();

    const uint16_t voltage_value = 20000;
    uint8_t read_buffer[] = {(voltage_value >> 8), voltage_value & 0xFF};
    twi_set_read_data(read_buffer);

    assert_int_equal(25000, ina260_read_voltage_in_mv(&ina260));
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x02,
            TWI_READ_NO_STOP, 0x40, 0x02,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

void test_read_power_in_mw(void) {
    struct INA260 ina260;
    ina260_init(&ina260, 0x40);
    twi_log_reset();

    const uint16_t power_value = 2000;
    uint8_t read_buffer[] = {(power_value >> 8), power_value & 0xFF};
    twi_set_read_data(read_buffer);

    assert_int_equal(20000, ina260_read_power_in_mw(&ina260));
    assert_buff_equal(
        ((uint8_t[]){
            TWI_START_WRITE, 0x40,
            TWI_WRITE_NO_STOP, 0x03,
            TWI_READ_NO_STOP, 0x40, 0x02,
            TWI_STOP,
        }),
        twi_log,
        twi_logidx
    );
}

int main(void) {
    test(test_basic_init);
    test(test_read_configuration);
    test(test_read_configuration_with_error);
    test(test_create_configuration);
    test(test_write_configuration);
    test(test_set_operating_mode);
    test(test_read_current_in_ma);
    test(test_read_voltage_in_mv);
    test(test_read_power_in_mw);
    return 0;
}
