#include "io_common.h"
#include <inttypes.h>
#include <string.h>

uint8_t ADCH_history[1024];
uint8_t ADCH_sequence[1024];
uint16_t ADCH_sequence_index;
uint16_t ADCH_sequence_max;
uint16_t ADCH_index;
volatile uint8_t ADCH_value;
volatile uint8_t* ADCH_() {
  ADCH_history[ADCH_index++] = ADCH_value;
  if (ADCH_sequence_index < ADCH_sequence_max) {
    ADCH_value = ADCH_sequence[ADCH_sequence_index++];
  }
  return &ADCH_value;
}

uint8_t ADCL_history[1024];
uint8_t ADCL_sequence[1024];
uint16_t ADCL_sequence_index;
uint16_t ADCL_sequence_max;
uint16_t ADCL_index;
volatile uint8_t ADCL_value;
volatile uint8_t* ADCL_() {
  ADCL_history[ADCL_index++] = ADCL_value;
  if (ADCL_sequence_index < ADCL_sequence_max) {
    ADCL_value = ADCL_sequence[ADCL_sequence_index++];
  }
  return &ADCL_value;
}

uint8_t ADCSRA_history[1024];
uint8_t ADCSRA_sequence[1024];
uint16_t ADCSRA_sequence_index;
uint16_t ADCSRA_sequence_max;
uint16_t ADCSRA_index;
volatile uint8_t ADCSRA_value;
volatile uint8_t* ADCSRA_() {
  ADCSRA_history[ADCSRA_index++] = ADCSRA_value;
  if (ADCSRA_sequence_index < ADCSRA_sequence_max) {
    ADCSRA_value = ADCSRA_sequence[ADCSRA_sequence_index++];
  }
  return &ADCSRA_value;
}

uint8_t ADMUX_history[1024];
uint8_t ADMUX_sequence[1024];
uint16_t ADMUX_sequence_index;
uint16_t ADMUX_sequence_max;
uint16_t ADMUX_index;
volatile uint8_t ADMUX_value;
volatile uint8_t* ADMUX_() {
  ADMUX_history[ADMUX_index++] = ADMUX_value;
  if (ADMUX_sequence_index < ADMUX_sequence_max) {
    ADMUX_value = ADMUX_sequence[ADMUX_sequence_index++];
  }
  return &ADMUX_value;
}

uint8_t DDRB_history[1024];
uint8_t DDRB_sequence[1024];
uint16_t DDRB_sequence_index;
uint16_t DDRB_sequence_max;
uint16_t DDRB_index;
volatile uint8_t DDRB_value;
volatile uint8_t* DDRB_() {
  DDRB_history[DDRB_index++] = DDRB_value;
  if (DDRB_sequence_index < DDRB_sequence_max) {
    DDRB_value = DDRB_sequence[DDRB_sequence_index++];
  }
  return &DDRB_value;
}

uint8_t DDRC_history[1024];
uint8_t DDRC_sequence[1024];
uint16_t DDRC_sequence_index;
uint16_t DDRC_sequence_max;
uint16_t DDRC_index;
volatile uint8_t DDRC_value;
volatile uint8_t* DDRC_() {
  DDRC_history[DDRC_index++] = DDRC_value;
  if (DDRC_sequence_index < DDRC_sequence_max) {
    DDRC_value = DDRC_sequence[DDRC_sequence_index++];
  }
  return &DDRC_value;
}

uint8_t DDRD_history[1024];
uint8_t DDRD_sequence[1024];
uint16_t DDRD_sequence_index;
uint16_t DDRD_sequence_max;
uint16_t DDRD_index;
volatile uint8_t DDRD_value;
volatile uint8_t* DDRD_() {
  DDRD_history[DDRD_index++] = DDRD_value;
  if (DDRD_sequence_index < DDRD_sequence_max) {
    DDRD_value = DDRD_sequence[DDRD_sequence_index++];
  }
  return &DDRD_value;
}

uint8_t GTCCR_history[1024];
uint8_t GTCCR_sequence[1024];
uint16_t GTCCR_sequence_index;
uint16_t GTCCR_sequence_max;
uint16_t GTCCR_index;
volatile uint8_t GTCCR_value;
volatile uint8_t* GTCCR_() {
  GTCCR_history[GTCCR_index++] = GTCCR_value;
  if (GTCCR_sequence_index < GTCCR_sequence_max) {
    GTCCR_value = GTCCR_sequence[GTCCR_sequence_index++];
  }
  return &GTCCR_value;
}

uint8_t OCR0A_history[1024];
uint8_t OCR0A_sequence[1024];
uint16_t OCR0A_sequence_index;
uint16_t OCR0A_sequence_max;
uint16_t OCR0A_index;
volatile uint8_t OCR0A_value;
volatile uint8_t* OCR0A_() {
  OCR0A_history[OCR0A_index++] = OCR0A_value;
  if (OCR0A_sequence_index < OCR0A_sequence_max) {
    OCR0A_value = OCR0A_sequence[OCR0A_sequence_index++];
  }
  return &OCR0A_value;
}

uint8_t OCR1A_history[1024];
uint8_t OCR1A_sequence[1024];
uint16_t OCR1A_sequence_index;
uint16_t OCR1A_sequence_max;
uint16_t OCR1A_index;
volatile uint8_t OCR1A_value;
volatile uint8_t* OCR1A_() {
  OCR1A_history[OCR1A_index++] = OCR1A_value;
  if (OCR1A_sequence_index < OCR1A_sequence_max) {
    OCR1A_value = OCR1A_sequence[OCR1A_sequence_index++];
  }
  return &OCR1A_value;
}

uint8_t OCR1AH_history[1024];
uint8_t OCR1AH_sequence[1024];
uint16_t OCR1AH_sequence_index;
uint16_t OCR1AH_sequence_max;
uint16_t OCR1AH_index;
volatile uint8_t OCR1AH_value;
volatile uint8_t* OCR1AH_() {
  OCR1AH_history[OCR1AH_index++] = OCR1AH_value;
  if (OCR1AH_sequence_index < OCR1AH_sequence_max) {
    OCR1AH_value = OCR1AH_sequence[OCR1AH_sequence_index++];
  }
  return &OCR1AH_value;
}

uint8_t OCR1AL_history[1024];
uint8_t OCR1AL_sequence[1024];
uint16_t OCR1AL_sequence_index;
uint16_t OCR1AL_sequence_max;
uint16_t OCR1AL_index;
volatile uint8_t OCR1AL_value;
volatile uint8_t* OCR1AL_() {
  OCR1AL_history[OCR1AL_index++] = OCR1AL_value;
  if (OCR1AL_sequence_index < OCR1AL_sequence_max) {
    OCR1AL_value = OCR1AL_sequence[OCR1AL_sequence_index++];
  }
  return &OCR1AL_value;
}

uint8_t PINA_history[1024];
uint8_t PINA_sequence[1024];
uint16_t PINA_sequence_index;
uint16_t PINA_sequence_max;
uint16_t PINA_index;
volatile uint8_t PINA_value;
volatile uint8_t* PINA_() {
  PINA_history[PINA_index++] = PINA_value;
  if (PINA_sequence_index < PINA_sequence_max) {
    PINA_value = PINA_sequence[PINA_sequence_index++];
  }
  return &PINA_value;
}

uint8_t PINB_history[1024];
uint8_t PINB_sequence[1024];
uint16_t PINB_sequence_index;
uint16_t PINB_sequence_max;
uint16_t PINB_index;
volatile uint8_t PINB_value;
volatile uint8_t* PINB_() {
  PINB_history[PINB_index++] = PINB_value;
  if (PINB_sequence_index < PINB_sequence_max) {
    PINB_value = PINB_sequence[PINB_sequence_index++];
  }
  return &PINB_value;
}

uint8_t PINC_history[1024];
uint8_t PINC_sequence[1024];
uint16_t PINC_sequence_index;
uint16_t PINC_sequence_max;
uint16_t PINC_index;
volatile uint8_t PINC_value;
volatile uint8_t* PINC_() {
  PINC_history[PINC_index++] = PINC_value;
  if (PINC_sequence_index < PINC_sequence_max) {
    PINC_value = PINC_sequence[PINC_sequence_index++];
  }
  return &PINC_value;
}

uint8_t PIND_history[1024];
uint8_t PIND_sequence[1024];
uint16_t PIND_sequence_index;
uint16_t PIND_sequence_max;
uint16_t PIND_index;
volatile uint8_t PIND_value;
volatile uint8_t* PIND_() {
  PIND_history[PIND_index++] = PIND_value;
  if (PIND_sequence_index < PIND_sequence_max) {
    PIND_value = PIND_sequence[PIND_sequence_index++];
  }
  return &PIND_value;
}

uint8_t PLLCSR_history[1024];
uint8_t PLLCSR_sequence[1024];
uint16_t PLLCSR_sequence_index;
uint16_t PLLCSR_sequence_max;
uint16_t PLLCSR_index;
volatile uint8_t PLLCSR_value;
volatile uint8_t* PLLCSR_() {
  PLLCSR_history[PLLCSR_index++] = PLLCSR_value;
  if (PLLCSR_sequence_index < PLLCSR_sequence_max) {
    PLLCSR_value = PLLCSR_sequence[PLLCSR_sequence_index++];
  }
  return &PLLCSR_value;
}

uint8_t PORTA_history[1024];
uint8_t PORTA_sequence[1024];
uint16_t PORTA_sequence_index;
uint16_t PORTA_sequence_max;
uint16_t PORTA_index;
volatile uint8_t PORTA_value;
volatile uint8_t* PORTA_() {
  PORTA_history[PORTA_index++] = PORTA_value;
  if (PORTA_sequence_index < PORTA_sequence_max) {
    PORTA_value = PORTA_sequence[PORTA_sequence_index++];
  }
  return &PORTA_value;
}

uint8_t PORTB_history[1024];
uint8_t PORTB_sequence[1024];
uint16_t PORTB_sequence_index;
uint16_t PORTB_sequence_max;
uint16_t PORTB_index;
volatile uint8_t PORTB_value;
volatile uint8_t* PORTB_() {
  PORTB_history[PORTB_index++] = PORTB_value;
  if (PORTB_sequence_index < PORTB_sequence_max) {
    PORTB_value = PORTB_sequence[PORTB_sequence_index++];
  }
  return &PORTB_value;
}

uint8_t PORTC_history[1024];
uint8_t PORTC_sequence[1024];
uint16_t PORTC_sequence_index;
uint16_t PORTC_sequence_max;
uint16_t PORTC_index;
volatile uint8_t PORTC_value;
volatile uint8_t* PORTC_() {
  PORTC_history[PORTC_index++] = PORTC_value;
  if (PORTC_sequence_index < PORTC_sequence_max) {
    PORTC_value = PORTC_sequence[PORTC_sequence_index++];
  }
  return &PORTC_value;
}

uint8_t PORTD_history[1024];
uint8_t PORTD_sequence[1024];
uint16_t PORTD_sequence_index;
uint16_t PORTD_sequence_max;
uint16_t PORTD_index;
volatile uint8_t PORTD_value;
volatile uint8_t* PORTD_() {
  PORTD_history[PORTD_index++] = PORTD_value;
  if (PORTD_sequence_index < PORTD_sequence_max) {
    PORTD_value = PORTD_sequence[PORTD_sequence_index++];
  }
  return &PORTD_value;
}

uint8_t SPCR_history[1024];
uint8_t SPCR_sequence[1024];
uint16_t SPCR_sequence_index;
uint16_t SPCR_sequence_max;
uint16_t SPCR_index;
volatile uint8_t SPCR_value;
volatile uint8_t* SPCR_() {
  SPCR_history[SPCR_index++] = SPCR_value;
  if (SPCR_sequence_index < SPCR_sequence_max) {
    SPCR_value = SPCR_sequence[SPCR_sequence_index++];
  }
  return &SPCR_value;
}

uint8_t SPDR_history[1024];
uint8_t SPDR_sequence[1024];
uint16_t SPDR_sequence_index;
uint16_t SPDR_sequence_max;
uint16_t SPDR_index;
volatile uint8_t SPDR_value;
volatile uint8_t* SPDR_() {
  SPDR_history[SPDR_index++] = SPDR_value;
  if (SPDR_sequence_index < SPDR_sequence_max) {
    SPDR_value = SPDR_sequence[SPDR_sequence_index++];
  }
  return &SPDR_value;
}

uint8_t SPSR_history[1024];
uint8_t SPSR_sequence[1024];
uint16_t SPSR_sequence_index;
uint16_t SPSR_sequence_max;
uint16_t SPSR_index;
volatile uint8_t SPSR_value;
volatile uint8_t* SPSR_() {
  SPSR_history[SPSR_index++] = SPSR_value;
  if (SPSR_sequence_index < SPSR_sequence_max) {
    SPSR_value = SPSR_sequence[SPSR_sequence_index++];
  }
  return &SPSR_value;
}

uint8_t SREG_history[1024];
uint8_t SREG_sequence[1024];
uint16_t SREG_sequence_index;
uint16_t SREG_sequence_max;
uint16_t SREG_index;
volatile uint8_t SREG_value;
volatile uint8_t* SREG_() {
  SREG_history[SREG_index++] = SREG_value;
  if (SREG_sequence_index < SREG_sequence_max) {
    SREG_value = SREG_sequence[SREG_sequence_index++];
  }
  return &SREG_value;
}

uint8_t TCCR0A_history[1024];
uint8_t TCCR0A_sequence[1024];
uint16_t TCCR0A_sequence_index;
uint16_t TCCR0A_sequence_max;
uint16_t TCCR0A_index;
volatile uint8_t TCCR0A_value;
volatile uint8_t* TCCR0A_() {
  TCCR0A_history[TCCR0A_index++] = TCCR0A_value;
  if (TCCR0A_sequence_index < TCCR0A_sequence_max) {
    TCCR0A_value = TCCR0A_sequence[TCCR0A_sequence_index++];
  }
  return &TCCR0A_value;
}

uint8_t TCCR0B_history[1024];
uint8_t TCCR0B_sequence[1024];
uint16_t TCCR0B_sequence_index;
uint16_t TCCR0B_sequence_max;
uint16_t TCCR0B_index;
volatile uint8_t TCCR0B_value;
volatile uint8_t* TCCR0B_() {
  TCCR0B_history[TCCR0B_index++] = TCCR0B_value;
  if (TCCR0B_sequence_index < TCCR0B_sequence_max) {
    TCCR0B_value = TCCR0B_sequence[TCCR0B_sequence_index++];
  }
  return &TCCR0B_value;
}

uint8_t TCCR1_history[1024];
uint8_t TCCR1_sequence[1024];
uint16_t TCCR1_sequence_index;
uint16_t TCCR1_sequence_max;
uint16_t TCCR1_index;
volatile uint8_t TCCR1_value;
volatile uint8_t* TCCR1_() {
  TCCR1_history[TCCR1_index++] = TCCR1_value;
  if (TCCR1_sequence_index < TCCR1_sequence_max) {
    TCCR1_value = TCCR1_sequence[TCCR1_sequence_index++];
  }
  return &TCCR1_value;
}

uint8_t TCCR1A_history[1024];
uint8_t TCCR1A_sequence[1024];
uint16_t TCCR1A_sequence_index;
uint16_t TCCR1A_sequence_max;
uint16_t TCCR1A_index;
volatile uint8_t TCCR1A_value;
volatile uint8_t* TCCR1A_() {
  TCCR1A_history[TCCR1A_index++] = TCCR1A_value;
  if (TCCR1A_sequence_index < TCCR1A_sequence_max) {
    TCCR1A_value = TCCR1A_sequence[TCCR1A_sequence_index++];
  }
  return &TCCR1A_value;
}

uint8_t TCCR1B_history[1024];
uint8_t TCCR1B_sequence[1024];
uint16_t TCCR1B_sequence_index;
uint16_t TCCR1B_sequence_max;
uint16_t TCCR1B_index;
volatile uint8_t TCCR1B_value;
volatile uint8_t* TCCR1B_() {
  TCCR1B_history[TCCR1B_index++] = TCCR1B_value;
  if (TCCR1B_sequence_index < TCCR1B_sequence_max) {
    TCCR1B_value = TCCR1B_sequence[TCCR1B_sequence_index++];
  }
  return &TCCR1B_value;
}

uint8_t TCNT0_history[1024];
uint8_t TCNT0_sequence[1024];
uint16_t TCNT0_sequence_index;
uint16_t TCNT0_sequence_max;
uint16_t TCNT0_index;
volatile uint8_t TCNT0_value;
volatile uint8_t* TCNT0_() {
  TCNT0_history[TCNT0_index++] = TCNT0_value;
  if (TCNT0_sequence_index < TCNT0_sequence_max) {
    TCNT0_value = TCNT0_sequence[TCNT0_sequence_index++];
  }
  return &TCNT0_value;
}

uint8_t TCNT1_history[1024];
uint8_t TCNT1_sequence[1024];
uint16_t TCNT1_sequence_index;
uint16_t TCNT1_sequence_max;
uint16_t TCNT1_index;
volatile uint8_t TCNT1_value;
volatile uint8_t* TCNT1_() {
  TCNT1_history[TCNT1_index++] = TCNT1_value;
  if (TCNT1_sequence_index < TCNT1_sequence_max) {
    TCNT1_value = TCNT1_sequence[TCNT1_sequence_index++];
  }
  return &TCNT1_value;
}

uint8_t TCNT1H_history[1024];
uint8_t TCNT1H_sequence[1024];
uint16_t TCNT1H_sequence_index;
uint16_t TCNT1H_sequence_max;
uint16_t TCNT1H_index;
volatile uint8_t TCNT1H_value;
volatile uint8_t* TCNT1H_() {
  TCNT1H_history[TCNT1H_index++] = TCNT1H_value;
  if (TCNT1H_sequence_index < TCNT1H_sequence_max) {
    TCNT1H_value = TCNT1H_sequence[TCNT1H_sequence_index++];
  }
  return &TCNT1H_value;
}

uint8_t TCNT1L_history[1024];
uint8_t TCNT1L_sequence[1024];
uint16_t TCNT1L_sequence_index;
uint16_t TCNT1L_sequence_max;
uint16_t TCNT1L_index;
volatile uint8_t TCNT1L_value;
volatile uint8_t* TCNT1L_() {
  TCNT1L_history[TCNT1L_index++] = TCNT1L_value;
  if (TCNT1L_sequence_index < TCNT1L_sequence_max) {
    TCNT1L_value = TCNT1L_sequence[TCNT1L_sequence_index++];
  }
  return &TCNT1L_value;
}

uint8_t TIFR0_history[1024];
uint8_t TIFR0_sequence[1024];
uint16_t TIFR0_sequence_index;
uint16_t TIFR0_sequence_max;
uint16_t TIFR0_index;
volatile uint8_t TIFR0_value;
volatile uint8_t* TIFR0_() {
  TIFR0_history[TIFR0_index++] = TIFR0_value;
  if (TIFR0_sequence_index < TIFR0_sequence_max) {
    TIFR0_value = TIFR0_sequence[TIFR0_sequence_index++];
  }
  return &TIFR0_value;
}

uint8_t TIMSK_history[1024];
uint8_t TIMSK_sequence[1024];
uint16_t TIMSK_sequence_index;
uint16_t TIMSK_sequence_max;
uint16_t TIMSK_index;
volatile uint8_t TIMSK_value;
volatile uint8_t* TIMSK_() {
  TIMSK_history[TIMSK_index++] = TIMSK_value;
  if (TIMSK_sequence_index < TIMSK_sequence_max) {
    TIMSK_value = TIMSK_sequence[TIMSK_sequence_index++];
  }
  return &TIMSK_value;
}

uint8_t TIMSK0_history[1024];
uint8_t TIMSK0_sequence[1024];
uint16_t TIMSK0_sequence_index;
uint16_t TIMSK0_sequence_max;
uint16_t TIMSK0_index;
volatile uint8_t TIMSK0_value;
volatile uint8_t* TIMSK0_() {
  TIMSK0_history[TIMSK0_index++] = TIMSK0_value;
  if (TIMSK0_sequence_index < TIMSK0_sequence_max) {
    TIMSK0_value = TIMSK0_sequence[TIMSK0_sequence_index++];
  }
  return &TIMSK0_value;
}

uint8_t TIMSK1_history[1024];
uint8_t TIMSK1_sequence[1024];
uint16_t TIMSK1_sequence_index;
uint16_t TIMSK1_sequence_max;
uint16_t TIMSK1_index;
volatile uint8_t TIMSK1_value;
volatile uint8_t* TIMSK1_() {
  TIMSK1_history[TIMSK1_index++] = TIMSK1_value;
  if (TIMSK1_sequence_index < TIMSK1_sequence_max) {
    TIMSK1_value = TIMSK1_sequence[TIMSK1_sequence_index++];
  }
  return &TIMSK1_value;
}

uint8_t TWBR_history[1024];
uint8_t TWBR_sequence[1024];
uint16_t TWBR_sequence_index;
uint16_t TWBR_sequence_max;
uint16_t TWBR_index;
volatile uint8_t TWBR_value;
volatile uint8_t* TWBR_() {
  TWBR_history[TWBR_index++] = TWBR_value;
  if (TWBR_sequence_index < TWBR_sequence_max) {
    TWBR_value = TWBR_sequence[TWBR_sequence_index++];
  }
  return &TWBR_value;
}

uint8_t TWCR_history[1024];
uint8_t TWCR_sequence[1024];
uint16_t TWCR_sequence_index;
uint16_t TWCR_sequence_max;
uint16_t TWCR_index;
volatile uint8_t TWCR_value;
volatile uint8_t* TWCR_() {
  TWCR_history[TWCR_index++] = TWCR_value;
  if (TWCR_sequence_index < TWCR_sequence_max) {
    TWCR_value = TWCR_sequence[TWCR_sequence_index++];
  }
  return &TWCR_value;
}

uint8_t TWSR_history[1024];
uint8_t TWSR_sequence[1024];
uint16_t TWSR_sequence_index;
uint16_t TWSR_sequence_max;
uint16_t TWSR_index;
volatile uint8_t TWSR_value;
volatile uint8_t* TWSR_() {
  TWSR_history[TWSR_index++] = TWSR_value;
  if (TWSR_sequence_index < TWSR_sequence_max) {
    TWSR_value = TWSR_sequence[TWSR_sequence_index++];
  }
  return &TWSR_value;
}

uint8_t UBRR0H_history[1024];
uint8_t UBRR0H_sequence[1024];
uint16_t UBRR0H_sequence_index;
uint16_t UBRR0H_sequence_max;
uint16_t UBRR0H_index;
volatile uint8_t UBRR0H_value;
volatile uint8_t* UBRR0H_() {
  UBRR0H_history[UBRR0H_index++] = UBRR0H_value;
  if (UBRR0H_sequence_index < UBRR0H_sequence_max) {
    UBRR0H_value = UBRR0H_sequence[UBRR0H_sequence_index++];
  }
  return &UBRR0H_value;
}

uint8_t UBRR0L_history[1024];
uint8_t UBRR0L_sequence[1024];
uint16_t UBRR0L_sequence_index;
uint16_t UBRR0L_sequence_max;
uint16_t UBRR0L_index;
volatile uint8_t UBRR0L_value;
volatile uint8_t* UBRR0L_() {
  UBRR0L_history[UBRR0L_index++] = UBRR0L_value;
  if (UBRR0L_sequence_index < UBRR0L_sequence_max) {
    UBRR0L_value = UBRR0L_sequence[UBRR0L_sequence_index++];
  }
  return &UBRR0L_value;
}

uint8_t UCSR0A_history[1024];
uint8_t UCSR0A_sequence[1024];
uint16_t UCSR0A_sequence_index;
uint16_t UCSR0A_sequence_max;
uint16_t UCSR0A_index;
volatile uint8_t UCSR0A_value;
volatile uint8_t* UCSR0A_() {
  UCSR0A_history[UCSR0A_index++] = UCSR0A_value;
  if (UCSR0A_sequence_index < UCSR0A_sequence_max) {
    UCSR0A_value = UCSR0A_sequence[UCSR0A_sequence_index++];
  }
  return &UCSR0A_value;
}

uint8_t UCSR0B_history[1024];
uint8_t UCSR0B_sequence[1024];
uint16_t UCSR0B_sequence_index;
uint16_t UCSR0B_sequence_max;
uint16_t UCSR0B_index;
volatile uint8_t UCSR0B_value;
volatile uint8_t* UCSR0B_() {
  UCSR0B_history[UCSR0B_index++] = UCSR0B_value;
  if (UCSR0B_sequence_index < UCSR0B_sequence_max) {
    UCSR0B_value = UCSR0B_sequence[UCSR0B_sequence_index++];
  }
  return &UCSR0B_value;
}

uint8_t UCSR0C_history[1024];
uint8_t UCSR0C_sequence[1024];
uint16_t UCSR0C_sequence_index;
uint16_t UCSR0C_sequence_max;
uint16_t UCSR0C_index;
volatile uint8_t UCSR0C_value;
volatile uint8_t* UCSR0C_() {
  UCSR0C_history[UCSR0C_index++] = UCSR0C_value;
  if (UCSR0C_sequence_index < UCSR0C_sequence_max) {
    UCSR0C_value = UCSR0C_sequence[UCSR0C_sequence_index++];
  }
  return &UCSR0C_value;
}

uint8_t UDR0_history[1024];
uint8_t UDR0_sequence[1024];
uint16_t UDR0_sequence_index;
uint16_t UDR0_sequence_max;
uint16_t UDR0_index;
volatile uint8_t UDR0_value;
volatile uint8_t* UDR0_() {
  UDR0_history[UDR0_index++] = UDR0_value;
  if (UDR0_sequence_index < UDR0_sequence_max) {
    UDR0_value = UDR0_sequence[UDR0_sequence_index++];
  }
  return &UDR0_value;
}

uint8_t USICR_history[1024];
uint8_t USICR_sequence[1024];
uint16_t USICR_sequence_index;
uint16_t USICR_sequence_max;
uint16_t USICR_index;
volatile uint8_t USICR_value;
volatile uint8_t* USICR_() {
  USICR_history[USICR_index++] = USICR_value;
  if (USICR_sequence_index < USICR_sequence_max) {
    USICR_value = USICR_sequence[USICR_sequence_index++];
  }
  return &USICR_value;
}

uint8_t USIDR_history[1024];
uint8_t USIDR_sequence[1024];
uint16_t USIDR_sequence_index;
uint16_t USIDR_sequence_max;
uint16_t USIDR_index;
volatile uint8_t USIDR_value;
volatile uint8_t* USIDR_() {
  USIDR_history[USIDR_index++] = USIDR_value;
  if (USIDR_sequence_index < USIDR_sequence_max) {
    USIDR_value = USIDR_sequence[USIDR_sequence_index++];
  }
  return &USIDR_value;
}

uint8_t USISR_history[1024];
uint8_t USISR_sequence[1024];
uint16_t USISR_sequence_index;
uint16_t USISR_sequence_max;
uint16_t USISR_index;
volatile uint8_t USISR_value;
volatile uint8_t* USISR_() {
  USISR_history[USISR_index++] = USISR_value;
  if (USISR_sequence_index < USISR_sequence_max) {
    USISR_value = USISR_sequence[USISR_sequence_index++];
  }
  return &USISR_value;
}

uint8_t TWDR_history[1024];
uint8_t TWDR_sequence[1024];
uint16_t TWDR_sequence_index;
uint16_t TWDR_sequence_max;
uint16_t TWDR_index;
volatile uint8_t TWDR_value;
volatile uint8_t* TWDR_() {
  TWDR_history[TWDR_index++] = TWDR_value;
  if (TWDR_sequence_index < TWDR_sequence_max) {
    TWDR_value = TWDR_sequence[TWDR_sequence_index++];
  }
  return &TWDR_value;
}

const unsigned int KNOWN_REGISTER_COUNT = 51;
uint16_t* const KNOWN_REGISTER_INDEX[] = {
  &ADCH_index,
  &ADCL_index,
  &ADCSRA_index,
  &ADMUX_index,
  &DDRB_index,
  &DDRC_index,
  &DDRD_index,
  &GTCCR_index,
  &OCR0A_index,
  &OCR1A_index,
  &OCR1AH_index,
  &OCR1AL_index,
  &PINA_index,
  &PINB_index,
  &PINC_index,
  &PIND_index,
  &PLLCSR_index,
  &PORTA_index,
  &PORTB_index,
  &PORTC_index,
  &PORTD_index,
  &SPCR_index,
  &SPDR_index,
  &SPSR_index,
  &SREG_index,
  &TCCR0A_index,
  &TCCR0B_index,
  &TCCR1_index,
  &TCCR1A_index,
  &TCCR1B_index,
  &TCNT0_index,
  &TCNT1_index,
  &TCNT1H_index,
  &TCNT1L_index,
  &TIFR0_index,
  &TIMSK_index,
  &TIMSK0_index,
  &TIMSK1_index,
  &TWBR_index,
  &TWCR_index,
  &TWSR_index,
  &UBRR0H_index,
  &UBRR0L_index,
  &UCSR0A_index,
  &UCSR0B_index,
  &UCSR0C_index,
  &UDR0_index,
  &USICR_index,
  &USIDR_index,
  &USISR_index,
  &TWDR_index,
};

const char* KNOWN_REGISTER_NAME[] = {
  "ADCH",
  "ADCL",
  "ADCSRA",
  "ADMUX",
  "DDRB",
  "DDRC",
  "DDRD",
  "GTCCR",
  "OCR0A",
  "OCR1A",
  "OCR1AH",
  "OCR1AL",
  "PINA",
  "PINB",
  "PINC",
  "PIND",
  "PLLCSR",
  "PORTA",
  "PORTB",
  "PORTC",
  "PORTD",
  "SPCR",
  "SPDR",
  "SPSR",
  "SREG",
  "TCCR0A",
  "TCCR0B",
  "TCCR1",
  "TCCR1A",
  "TCCR1B",
  "TCNT0",
  "TCNT1",
  "TCNT1H",
  "TCNT1L",
  "TIFR0",
  "TIMSK",
  "TIMSK0",
  "TIMSK1",
  "TWBR",
  "TWCR",
  "TWSR",
  "UBRR0H",
  "UBRR0L",
  "UCSR0A",
  "UCSR0B",
  "UCSR0C",
  "UDR0",
  "USICR",
  "USIDR",
  "USISR",
  "TWDR",
};

void avr_reset(void) {
  ADCH_value = 0;
  ADCH_index = 0;
  ADCH_sequence_index = 0;
  ADCH_sequence_max = 0;
  memset(ADCH_history, 0, sizeof(ADCH_history));

  ADCL_value = 0;
  ADCL_index = 0;
  ADCL_sequence_index = 0;
  ADCL_sequence_max = 0;
  memset(ADCL_history, 0, sizeof(ADCL_history));

  ADCSRA_value = 0;
  ADCSRA_index = 0;
  ADCSRA_sequence_index = 0;
  ADCSRA_sequence_max = 0;
  memset(ADCSRA_history, 0, sizeof(ADCSRA_history));

  ADMUX_value = 0;
  ADMUX_index = 0;
  ADMUX_sequence_index = 0;
  ADMUX_sequence_max = 0;
  memset(ADMUX_history, 0, sizeof(ADMUX_history));

  DDRB_value = 0;
  DDRB_index = 0;
  DDRB_sequence_index = 0;
  DDRB_sequence_max = 0;
  memset(DDRB_history, 0, sizeof(DDRB_history));

  DDRC_value = 0;
  DDRC_index = 0;
  DDRC_sequence_index = 0;
  DDRC_sequence_max = 0;
  memset(DDRC_history, 0, sizeof(DDRC_history));

  DDRD_value = 0;
  DDRD_index = 0;
  DDRD_sequence_index = 0;
  DDRD_sequence_max = 0;
  memset(DDRD_history, 0, sizeof(DDRD_history));

  GTCCR_value = 0;
  GTCCR_index = 0;
  GTCCR_sequence_index = 0;
  GTCCR_sequence_max = 0;
  memset(GTCCR_history, 0, sizeof(GTCCR_history));

  OCR0A_value = 0;
  OCR0A_index = 0;
  OCR0A_sequence_index = 0;
  OCR0A_sequence_max = 0;
  memset(OCR0A_history, 0, sizeof(OCR0A_history));

  OCR1A_value = 0;
  OCR1A_index = 0;
  OCR1A_sequence_index = 0;
  OCR1A_sequence_max = 0;
  memset(OCR1A_history, 0, sizeof(OCR1A_history));

  OCR1AH_value = 0;
  OCR1AH_index = 0;
  OCR1AH_sequence_index = 0;
  OCR1AH_sequence_max = 0;
  memset(OCR1AH_history, 0, sizeof(OCR1AH_history));

  OCR1AL_value = 0;
  OCR1AL_index = 0;
  OCR1AL_sequence_index = 0;
  OCR1AL_sequence_max = 0;
  memset(OCR1AL_history, 0, sizeof(OCR1AL_history));

  PINA_value = 0;
  PINA_index = 0;
  PINA_sequence_index = 0;
  PINA_sequence_max = 0;
  memset(PINA_history, 0, sizeof(PINA_history));

  PINB_value = 0;
  PINB_index = 0;
  PINB_sequence_index = 0;
  PINB_sequence_max = 0;
  memset(PINB_history, 0, sizeof(PINB_history));

  PINC_value = 0;
  PINC_index = 0;
  PINC_sequence_index = 0;
  PINC_sequence_max = 0;
  memset(PINC_history, 0, sizeof(PINC_history));

  PIND_value = 0;
  PIND_index = 0;
  PIND_sequence_index = 0;
  PIND_sequence_max = 0;
  memset(PIND_history, 0, sizeof(PIND_history));

  PLLCSR_value = 0;
  PLLCSR_index = 0;
  PLLCSR_sequence_index = 0;
  PLLCSR_sequence_max = 0;
  memset(PLLCSR_history, 0, sizeof(PLLCSR_history));

  PORTA_value = 0;
  PORTA_index = 0;
  PORTA_sequence_index = 0;
  PORTA_sequence_max = 0;
  memset(PORTA_history, 0, sizeof(PORTA_history));

  PORTB_value = 0;
  PORTB_index = 0;
  PORTB_sequence_index = 0;
  PORTB_sequence_max = 0;
  memset(PORTB_history, 0, sizeof(PORTB_history));

  PORTC_value = 0;
  PORTC_index = 0;
  PORTC_sequence_index = 0;
  PORTC_sequence_max = 0;
  memset(PORTC_history, 0, sizeof(PORTC_history));

  PORTD_value = 0;
  PORTD_index = 0;
  PORTD_sequence_index = 0;
  PORTD_sequence_max = 0;
  memset(PORTD_history, 0, sizeof(PORTD_history));

  SPCR_value = 0;
  SPCR_index = 0;
  SPCR_sequence_index = 0;
  SPCR_sequence_max = 0;
  memset(SPCR_history, 0, sizeof(SPCR_history));

  SPDR_value = 0;
  SPDR_index = 0;
  SPDR_sequence_index = 0;
  SPDR_sequence_max = 0;
  memset(SPDR_history, 0, sizeof(SPDR_history));

  SPSR_value = 0;
  SPSR_index = 0;
  SPSR_sequence_index = 0;
  SPSR_sequence_max = 0;
  memset(SPSR_history, 0, sizeof(SPSR_history));

  SREG_value = 0;
  SREG_index = 0;
  SREG_sequence_index = 0;
  SREG_sequence_max = 0;
  memset(SREG_history, 0, sizeof(SREG_history));

  TCCR0A_value = 0;
  TCCR0A_index = 0;
  TCCR0A_sequence_index = 0;
  TCCR0A_sequence_max = 0;
  memset(TCCR0A_history, 0, sizeof(TCCR0A_history));

  TCCR0B_value = 0;
  TCCR0B_index = 0;
  TCCR0B_sequence_index = 0;
  TCCR0B_sequence_max = 0;
  memset(TCCR0B_history, 0, sizeof(TCCR0B_history));

  TCCR1_value = 0;
  TCCR1_index = 0;
  TCCR1_sequence_index = 0;
  TCCR1_sequence_max = 0;
  memset(TCCR1_history, 0, sizeof(TCCR1_history));

  TCCR1A_value = 0;
  TCCR1A_index = 0;
  TCCR1A_sequence_index = 0;
  TCCR1A_sequence_max = 0;
  memset(TCCR1A_history, 0, sizeof(TCCR1A_history));

  TCCR1B_value = 0;
  TCCR1B_index = 0;
  TCCR1B_sequence_index = 0;
  TCCR1B_sequence_max = 0;
  memset(TCCR1B_history, 0, sizeof(TCCR1B_history));

  TCNT0_value = 0;
  TCNT0_index = 0;
  TCNT0_sequence_index = 0;
  TCNT0_sequence_max = 0;
  memset(TCNT0_history, 0, sizeof(TCNT0_history));

  TCNT1_value = 0;
  TCNT1_index = 0;
  TCNT1_sequence_index = 0;
  TCNT1_sequence_max = 0;
  memset(TCNT1_history, 0, sizeof(TCNT1_history));

  TCNT1H_value = 0;
  TCNT1H_index = 0;
  TCNT1H_sequence_index = 0;
  TCNT1H_sequence_max = 0;
  memset(TCNT1H_history, 0, sizeof(TCNT1H_history));

  TCNT1L_value = 0;
  TCNT1L_index = 0;
  TCNT1L_sequence_index = 0;
  TCNT1L_sequence_max = 0;
  memset(TCNT1L_history, 0, sizeof(TCNT1L_history));

  TIFR0_value = 0;
  TIFR0_index = 0;
  TIFR0_sequence_index = 0;
  TIFR0_sequence_max = 0;
  memset(TIFR0_history, 0, sizeof(TIFR0_history));

  TIMSK_value = 0;
  TIMSK_index = 0;
  TIMSK_sequence_index = 0;
  TIMSK_sequence_max = 0;
  memset(TIMSK_history, 0, sizeof(TIMSK_history));

  TIMSK0_value = 0;
  TIMSK0_index = 0;
  TIMSK0_sequence_index = 0;
  TIMSK0_sequence_max = 0;
  memset(TIMSK0_history, 0, sizeof(TIMSK0_history));

  TIMSK1_value = 0;
  TIMSK1_index = 0;
  TIMSK1_sequence_index = 0;
  TIMSK1_sequence_max = 0;
  memset(TIMSK1_history, 0, sizeof(TIMSK1_history));

  TWBR_value = 0;
  TWBR_index = 0;
  TWBR_sequence_index = 0;
  TWBR_sequence_max = 0;
  memset(TWBR_history, 0, sizeof(TWBR_history));

  TWCR_value = 0;
  TWCR_index = 0;
  TWCR_sequence_index = 0;
  TWCR_sequence_max = 0;
  memset(TWCR_history, 0, sizeof(TWCR_history));

  TWSR_value = 0;
  TWSR_index = 0;
  TWSR_sequence_index = 0;
  TWSR_sequence_max = 0;
  memset(TWSR_history, 0, sizeof(TWSR_history));

  UBRR0H_value = 0;
  UBRR0H_index = 0;
  UBRR0H_sequence_index = 0;
  UBRR0H_sequence_max = 0;
  memset(UBRR0H_history, 0, sizeof(UBRR0H_history));

  UBRR0L_value = 0;
  UBRR0L_index = 0;
  UBRR0L_sequence_index = 0;
  UBRR0L_sequence_max = 0;
  memset(UBRR0L_history, 0, sizeof(UBRR0L_history));

  UCSR0A_value = (1 << UDRE0);
  UCSR0A_index = 0;
  UCSR0A_sequence_index = 0;
  UCSR0A_sequence_max = 0;
  memset(UCSR0A_history, 0, sizeof(UCSR0A_history));

  UCSR0B_value = 0;
  UCSR0B_index = 0;
  UCSR0B_sequence_index = 0;
  UCSR0B_sequence_max = 0;
  memset(UCSR0B_history, 0, sizeof(UCSR0B_history));

  UCSR0C_value = 0;
  UCSR0C_index = 0;
  UCSR0C_sequence_index = 0;
  UCSR0C_sequence_max = 0;
  memset(UCSR0C_history, 0, sizeof(UCSR0C_history));

  UDR0_value = 0;
  UDR0_index = 0;
  UDR0_sequence_index = 0;
  UDR0_sequence_max = 0;
  memset(UDR0_history, 0, sizeof(UDR0_history));

  USICR_value = 0;
  USICR_index = 0;
  USICR_sequence_index = 0;
  USICR_sequence_max = 0;
  memset(USICR_history, 0, sizeof(USICR_history));

  USIDR_value = 0;
  USIDR_index = 0;
  USIDR_sequence_index = 0;
  USIDR_sequence_max = 0;
  memset(USIDR_history, 0, sizeof(USIDR_history));

  USISR_value = 0;
  USISR_index = 0;
  USISR_sequence_index = 0;
  USISR_sequence_max = 0;
  memset(USISR_history, 0, sizeof(USISR_history));

  TWDR_value = 0;
  TWDR_index = 0;
  TWDR_sequence_index = 0;
  TWDR_sequence_max = 0;
  memset(TWDR_history, 0, sizeof(TWDR_history));

}
