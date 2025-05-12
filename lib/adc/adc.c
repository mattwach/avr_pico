#include "adc.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define PRESCALER ADC_PRESCALER_2

static inline uint8_t _get_bit() {
  ADCSRA = (1 << ADEN) | (1 << ADSC);
  while (ADCSRA & (1<<ADSC));
  return ADCH & 1;
}

uint16_t adc_quiet_read16(
    uint8_t adc_channel,
    uint8_t reference,
    uint8_t prescaler) {
  ADCSRA = (1 << ADEN) | (1 << ADIF);  // Enable ADC, cancel any pending interrupt
  ADCSRA |= prescaler;
  ADMUX = reference | adc_channel;

  cli();
  set_sleep_mode(SLEEP_MODE_ADC);
  sleep_enable();

  // start the conversion
  ADCSRA |= (1 << ADSC) | (1 << ADIE);
  sei();
  sleep_cpu();
  sleep_disable();

  // we should be awake due to ADC being done, but maybe it was some other
  // source
  while (ADCSRA & (1<<ADSC));

  return ADC;
}

uint16_t adc_quiet_read16_internal_ref(
    uint8_t prescaler) {
  ADCSRA = (1 << ADEN) | (1 << ADIF);  // Enable ADC, cancel any pending interrupt
  ADCSRA |= prescaler;
  ADMUX = ADC_REF_VCC | 0xC0;  // Use the internal 1.1V VREF

  cli();
  set_sleep_mode(SLEEP_MODE_ADC);
  sleep_enable();

  // start the conversion
  ADCSRA |= (1 << ADIE);
  _delay_ms(1);  // The datasheet calls for a 1ms settling time.
  ADCSRA |= (1 << ADSC);
  sei();
  sleep_cpu();
  sleep_disable();

  // we should be awake due to ADC being done, but maybe it was some other
  // source
  while (ADCSRA & (1<<ADSC));

  return ADC;
}

static uint8_t _entropy8(uint8_t bits, uint8_t rounds) {
  uint8_t result = 0x00;

  for (uint8_t bit = 0; bit < bits; ++bit) {
    result <<= 1;
    for (uint16_t round = 0; round < rounds; ++round) {
      result ^= _get_bit();
      const uint8_t maxr = round - 1;
      for (uint16_t i=0; i < maxr; ++i) {
        _delay_us(1);
      }
    }
  }

  return result;
}

uint8_t adc_entropy8(uint8_t adc_channel, uint8_t bits, uint8_t rounds) {
  if (bits > 8) {
    bits = 8;
  }

  // Note if ADC was enabled
  uint8_t was_enabled = ADCSRA & (1 << ADEN);

  ADMUX = ADC_REF_VCC | (1 << ADLAR) | adc_channel | PRESCALER;
  uint8_t result = _entropy8(bits, rounds);

  // Restore previous setting
  if (!was_enabled) {
    ADCSRA &= ~(1 << ADEN);
  }

  return result;
}

uint16_t adc_entropy16(uint8_t adc_channel, uint8_t bits, uint8_t rounds) {
  if (bits > 16) {
    bits = 16;
  }

  // Note if ADC was enabled
  uint8_t was_enabled = ADCSRA & (1 << ADEN);

  ADMUX = ADC_REF_VCC | (1 << ADLAR) | adc_channel | PRESCALER;
  uint16_t result = 0;
  if (bits > 8) {
    bits -= 8;
    result = (uint16_t)_entropy8(8, rounds) << bits;
  }
  result |= _entropy8(bits, rounds);

  // Restore previous setting
  if (!was_enabled) {
    ADCSRA &= ~(1 << ADEN);
  }

  return result;
}

uint32_t adc_entropy32(uint8_t adc_channel, uint8_t bits, uint8_t rounds) {
  if (bits > 32) {
    bits = 32;
  }

  // Note if ADC was enabled
  uint8_t was_enabled = ADCSRA & (1 << ADEN);

  ADMUX = ADC_REF_VCC | (1 << ADLAR) | adc_channel | PRESCALER;
  uint32_t result = 0;
  while (bits > 8) {
    bits -= 8;
    result = (result | (uint32_t)_entropy8(8, rounds)) << (bits > 8 ? 8 : bits);
  }
  result |= _entropy8(bits, rounds);

  // Restore previous setting
  if (!was_enabled) {
    ADCSRA &= ~(1 << ADEN);
  }

  return result;
}

ISR(ADC_vect)
{
  // empty interrupt
}
