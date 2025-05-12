#include "adc.h"

#include <test/unit_test.h>

void read8_test_helper(
    uint8_t adc_channel,
    uint8_t reference,
    uint8_t prescaler,
    uint8_t expected_admux) {
    reset_history();
    sequence(ADCSRA, 0x00); // conversion write
    sequence(ADCSRA, 0x00); // ADSC cleared 
    sequence(ADCH, 0xAB);  // ADC Value read

    uint8_t val = adc_read8(adc_channel, reference, prescaler);
    assert_int_equal(0xAB, val);
    assert_reg_equal(expected_admux, ADMUX);
    assert_history(ADCSRA, 0xC0 | prescaler, 0x00);
}

void read16_test_helper(
    uint8_t adc_channel,
    uint8_t reference,
    uint8_t prescaler,
    uint8_t expected_admux) {
    reset_history();
    sequence(ADCSRA, 0x00); // conversion write
    sequence(ADCSRA, 0x00); // ADSC cleared 
    sequence(ADCH, 0x02);  // ADC Value read
    sequence(ADCL, 0xAB);  // ADC Value read

    uint16_t val = adc_read16(adc_channel, reference, prescaler);
    assert_int_equal(0x2AB, val);
    assert_reg_equal(expected_admux, ADMUX);
    assert_history(ADCSRA, 0xC0 | prescaler, 0x00);
}

void test_read8(void) {
  read8_test_helper(0, ADC_REF_VCC, ADC_PRESCALER_128, 0x20);
  read8_test_helper(1, ADC_REF_AREF, ADC_PRESCALER_64, 0x61);
  read8_test_helper(2, ADC_REF_INTERNAL11, ADC_PRESCALER_32, 0xA2);
  read8_test_helper(3, ADC_REF_INTERNAL256, ADC_PRESCALER_16, 0xB3);
  read8_test_helper(0, ADC_REF_INTERNAL256_BYPASS, ADC_PRESCALER_16, 0xF0);
}

void test_read16(void) {
  read16_test_helper(0, ADC_REF_VCC, ADC_PRESCALER_128, 0x00);
  read16_test_helper(1, ADC_REF_AREF, ADC_PRESCALER_64, 0x41);
  read16_test_helper(2, ADC_REF_INTERNAL11, ADC_PRESCALER_32, 0x82);
  read16_test_helper(3, ADC_REF_INTERNAL256, ADC_PRESCALER_16, 0x93);
  read16_test_helper(0, ADC_REF_INTERNAL256_BYPASS, ADC_PRESCALER_16, 0xD0);
}

void test_entropy8_sample(void) {
  sequence(ADCSRA, 0x80); // Initial was_enabled check
  for (int i=0; i < 8; ++i) {
    sequence(ADCSRA, 0x00); // conversion write
    sequence(ADCSRA, 0x00); // ADSC cleared 
  }

  sequence(ADCH, 0xCF);  // 1
  sequence(ADCH, 0xC0);  // 0
  sequence(ADCH, 0x17);  // 1
  sequence(ADCH, 0xAD);  // 1

  sequence(ADCH, 0xEA);  // 0
  sequence(ADCH, 0xDB);  // 1
  sequence(ADCH, 0xF8);  // 0
  sequence(ADCH, 0xCC);  // 0

  uint8_t val = adc_entropy8(2, 8, 1);
  assert_reg_equal(0x22, ADMUX);
  assert_int_equal(0xB4, val);  // See sequence above
}

void test_entropy8_rounds(void) {
  sequence(ADCSRA, 0x80); // Initial was_enabled check
  for (int i=0; i < 8; ++i) {
    sequence(ADCSRA, 0x00); // conversion write
    sequence(ADCSRA, 0x00); // ADSC cleared 
  }

  // Bit one rounds -> 1 ^ 0 ^ 1 ^ 1 -> 1
  sequence(ADCH, 0xCF);  // 1
  sequence(ADCH, 0xC0);  // 0
  sequence(ADCH, 0x17);  // 1
  sequence(ADCH, 0xAD);  // 1

  // Bit zero rounds -> 0 ^ 1 ^ 1 ^ 0 -> 0
  sequence(ADCH, 0xEA);  // 0
  sequence(ADCH, 0xDB);  // 1
  sequence(ADCH, 0xF9);  // 1
  sequence(ADCH, 0xCC);  // 0

  uint8_t val = adc_entropy8(2, 2, 4);
  assert_reg_equal(0x22, ADMUX);
  assert_int_equal(0x02, val);  // See sequence above
}

void test_entropy8_bit_walk(void) {
  uint8_t result = 0;
  ADCH_value = 0x01; // Always return a 1

  for (uint8_t bits = 0; bits < 10; ++bits) {
    reset_history();
    sequence(ADCSRA, 0x80); // Initial was_enabled check
    for (int i=0; i < bits; ++i) {
      sequence(ADCSRA, 0x00); // conversion write
      sequence(ADCSRA, 0x00); // ADSC cleared 
    }

    uint8_t val = adc_entropy8(2, bits, 1);
    assert_int_equal(result, val);  // See sequence above
    result = result << 1 | 0x01;
  }

  assert_reg_activity(ADMUX);
  assert_reg_activity(ADCH);
}

void test_entropy16_sample(void) {
  sequence(ADCSRA, 0x80); // Initial was_enabled check
  for (int i=0; i < 16; ++i) {
    sequence(ADCSRA, 0x00); // conversion write
    sequence(ADCSRA, 0x00); // ADSC cleared 
  }

  sequence(ADCH, 0x29); // 1
  sequence(ADCH, 0x9D); // 1
  sequence(ADCH, 0x61); // 1
  sequence(ADCH, 0x1D); // 1
  
  sequence(ADCH, 0xE2); // 0
  sequence(ADCH, 0xAB); // 1
  sequence(ADCH, 0x0A); // 0
  sequence(ADCH, 0xEA); // 0
  
  sequence(ADCH, 0x14); // 0
  sequence(ADCH, 0x93); // 1
  sequence(ADCH, 0x2D); // 1
  sequence(ADCH, 0x43); // 1

  sequence(ADCH, 0x30); // 0
  sequence(ADCH, 0x0D); // 1
  sequence(ADCH, 0xE6); // 0
  sequence(ADCH, 0xE4); // 0

  uint16_t val = adc_entropy16(4, 16, 1);
  assert_reg_equal(0x24, ADMUX);
  assert_int_equal(0xF474, val);  // See sequence above
}

void test_entropy16_bit_walk(void) {
  uint16_t result = 0;
  ADCH_value = 0x01; // Always return a 1

  for (uint8_t bits = 0; bits < 18; ++bits) {
    reset_history();
    sequence(ADCSRA, 0x80); // Initial was_enabled check
    sequence(ADCSRA, 0x00); // conversion write
    for (int i=0; i < 80; ++i) {
      sequence(ADCSRA, 0x00); // conversion write
      sequence(ADCSRA, 0x00); // ADSC cleared 
    }

    uint16_t val = adc_entropy16(2, bits, 1);
    assert_int_equal(result, val);  // See sequence above
    result = result << 1 | 0x01;
  }

  assert_reg_activity(ADMUX);
  assert_reg_activity(ADCH);
}

void test_entropy32_sample(void) {
  sequence(ADCSRA, 0x80); // Initial was_enabled check
  for (int i=0; i < 32; ++i) {
    sequence(ADCSRA, 0x00); // conversion write
    sequence(ADCSRA, 0x00); // ADSC cleared 
  }

  sequence(ADCH, 0x0D); // 1
  sequence(ADCH, 0x7E); // 0
  sequence(ADCH, 0xAE); // 0
  sequence(ADCH, 0xA9); // 1

  sequence(ADCH, 0xAE); // 0
  sequence(ADCH, 0x37); // 1
  sequence(ADCH, 0xD5); // 1
  sequence(ADCH, 0x1A); // 0

  sequence(ADCH, 0xA6); // 0
  sequence(ADCH, 0x66); // 0
  sequence(ADCH, 0x21); // 1
  sequence(ADCH, 0xA4); // 0

  sequence(ADCH, 0xF9); // 1
  sequence(ADCH, 0x24); // 0
  sequence(ADCH, 0xA7); // 1
  sequence(ADCH, 0x61); // 1

  sequence(ADCH, 0xC9); // 1
  sequence(ADCH, 0x07); // 1
  sequence(ADCH, 0x4D); // 1
  sequence(ADCH, 0xF8); // 0

  sequence(ADCH, 0xC0); // 0
  sequence(ADCH, 0xAC); // 0
  sequence(ADCH, 0x76); // 0
  sequence(ADCH, 0x77); // 1

  sequence(ADCH, 0xD5); // 1
  sequence(ADCH, 0x22); // 0
  sequence(ADCH, 0x83); // 1
  sequence(ADCH, 0x04); // 0

  sequence(ADCH, 0x7F); // 1
  sequence(ADCH, 0xED); // 1
  sequence(ADCH, 0xBA); // 0
  sequence(ADCH, 0xD6); // 0

  uint32_t val = adc_entropy32(4, 32, 1);
  assert_reg_equal(0x24, ADMUX);
  assert_int_equal(0x962BE1AC, val);  // See sequence above
}

void test_entropy32_bit_walk(void) {
  uint32_t result = 0;
  ADCH_value = 0x01; // Always return a 1

  for (uint8_t bits = 0; bits < 34; ++bits) {
    reset_history();
    sequence(ADCSRA, 0x80); // Initial was_enabled check
    sequence(ADCSRA, 0x00); // conversion write
    for (int i=0; i < 80; ++i) {
      sequence(ADCSRA, 0x00); // conversion write
      sequence(ADCSRA, 0x00); // ADSC cleared 
    }

    uint32_t val = adc_entropy32(2, bits, 1);
    assert_int_equal(result, val);  // See sequence above
    result = result << 1 | 0x01;
  }

  assert_reg_activity(ADMUX);
  assert_reg_activity(ADCH);
}


int main(void) {
  test(test_read8);
  test(test_read16);

  test(test_entropy8_sample);
  test(test_entropy8_bit_walk);

  test(test_entropy8_rounds);

  test(test_entropy16_sample);
  test(test_entropy16_bit_walk);

  test(test_entropy32_sample);
  test(test_entropy32_bit_walk);

  return 0;
}
