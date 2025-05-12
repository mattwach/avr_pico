#ifndef AVR_IO_H
#define AVR_IO_H

#include "io_common.h"

// Useful defines
#define ISR(fn) void fn(void)


// Normal Hardware Registers

extern uint8_t* ADCH_();
#define ADCH *(ADCH_())

extern uint8_t* ADCL_();
#define ADCL *(ADCL_())

extern uint8_t* ADCSRA_();
#define ADCSRA *(ADCSRA_())

extern uint8_t* ADMUX_();
#define ADMUX *(ADMUX_())

extern uint8_t* DDRB_();
#define DDRB *(DDRB_())

extern uint8_t* DDRC_();
#define DDRC *(DDRC_())

extern uint8_t* DDRD_();
#define DDRD *(DDRD_())

extern uint8_t* GTCCR_();
#define GTCCR *(GTCCR_())

extern uint8_t* OCR0A_();
#define OCR0A *(OCR0A_())

extern uint8_t* OCR1A_();
#define OCR1A *(OCR1A_())

extern uint8_t* OCR1AH_();
#define OCR1AH *(OCR1AH_())

extern uint8_t* OCR1AL_();
#define OCR1AL *(OCR1AL_())

extern uint8_t* PINA_();
#define PINA *(PINA_())

extern uint8_t* PINB_();
#define PINB *(PINB_())

extern uint8_t* PINC_();
#define PINC *(PINC_())

extern uint8_t* PIND_();
#define PIND *(PIND_())

extern uint8_t* PLLCSR_();
#define PLLCSR *(PLLCSR_())

extern uint8_t* PORTA_();
#define PORTA *(PORTA_())

extern uint8_t* PORTB_();
#define PORTB *(PORTB_())

extern uint8_t* PORTC_();
#define PORTC *(PORTC_())

extern uint8_t* PORTD_();
#define PORTD *(PORTD_())

extern uint8_t* SPCR_();
#define SPCR *(SPCR_())

extern uint8_t* SPDR_();
#define SPDR *(SPDR_())

extern uint8_t* SPSR_();
#define SPSR *(SPSR_())

extern uint8_t* SREG_();
#define SREG *(SREG_())

extern uint8_t* TCCR0A_();
#define TCCR0A *(TCCR0A_())

extern uint8_t* TCCR0B_();
#define TCCR0B *(TCCR0B_())

extern uint8_t* TCCR1_();
#define TCCR1 *(TCCR1_())

extern uint8_t* TCCR1A_();
#define TCCR1A *(TCCR1A_())

extern uint8_t* TCCR1B_();
#define TCCR1B *(TCCR1B_())

extern uint8_t* TCNT0_();
#define TCNT0 *(TCNT0_())

extern uint8_t* TCNT1_();
#define TCNT1 *(TCNT1_())

extern uint8_t* TCNT1H_();
#define TCNT1H *(TCNT1H_())

extern uint8_t* TCNT1L_();
#define TCNT1L *(TCNT1L_())

extern uint8_t* TIFR0_();
#define TIFR0 *(TIFR0_())

extern uint8_t* TIMSK_();
#define TIMSK *(TIMSK_())

extern uint8_t* TIMSK0_();
#define TIMSK0 *(TIMSK0_())

extern uint8_t* TIMSK1_();
#define TIMSK1 *(TIMSK1_())

extern uint8_t* TWBR_();
#define TWBR *(TWBR_())

extern uint8_t* TWCR_();
#define TWCR *(TWCR_())

extern uint8_t* TWSR_();
#define TWSR *(TWSR_())

extern uint8_t* UBRR0H_();
#define UBRR0H *(UBRR0H_())

extern uint8_t* UBRR0L_();
#define UBRR0L *(UBRR0L_())

extern uint8_t* UCSR0A_();
#define UCSR0A *(UCSR0A_())

extern uint8_t* UCSR0B_();
#define UCSR0B *(UCSR0B_())

extern uint8_t* UCSR0C_();
#define UCSR0C *(UCSR0C_())

extern uint8_t* UDR0_();
#define UDR0 *(UDR0_())

extern uint8_t* USICR_();
#define USICR *(USICR_())

extern uint8_t* USIDR_();
#define USIDR *(USIDR_())

extern uint8_t* USISR_();
#define USISR *(USISR_())

// Interrupt Triggering Hardware Registers

#define TWDR "Do not access TWDR directly, please define a GET_TWDR, SET_TWDR (test/Readme.md)"
extern uint8_t* TWDR_();
#ifdef USE_TWI_TX_vect
void TWI_TX_vect(void);
#define SET_TWDR(v) *(TWDR_()) = (v); if ((SREG_value & 0x80) && (TWCR_value & (1<<TWINT))) { TWI_TX_vect(); }
#else
#define SET_TWDR(v) *(TWDR_()) = (v)
#endif
#define GET_TWDR() *(TWDR_())



// Macros

#define ADC ((ADCH) << 8 | (ADCL))

#endif  // AVR_IO_H
