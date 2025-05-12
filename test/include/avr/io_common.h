#ifndef AVR_IO_COMMON_H
#define AVR_IO_COMMON_H

#include <inttypes.h>


// Bit Definitions

#define ADEN 7
#define ADIE 3
#define ADIF 4
#define ADSC 6
#define ADLAR 5
#define CPHA 2
#define CPOL 3
#define DORD 5
#define MSTR 4
#define OCIE0A 4
#define OCIE1A 1
#define PINB0 0
#define PINB1 1
#define PINB2 2
#define PINB3 3
#define PINB4 4
#define PINB5 5
#define PINB6 6
#define PINB7 7
#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5
#define PORTB6 6
#define PORTB7 7
#define RXC0 7
#define RXCIE0 7
#define RXEN0 4
#define SPE 6
#define SPI2X 0
#define SPIF 7
#define SPR0 0
#define SPR1 1
#define TWEA 6
#define TWEN 2
#define TWINT 7
#define TWPS0 0
#define TWPS1 1
#define TWSTA 5
#define TWSTO 4
#define TXCIE0 6
#define TXEN0 3
#define U2X0 1
#define UCSZ00 1
#define UCSZ01 2
#define UDRE0 5
#define USICLK 1
#define USICNT0 0
#define USICNT1 1
#define USICNT2 2
#define USICNT3 3
#define USICS0 2
#define USICS1 3
#define USIDC 4
#define USIOIE 6
#define USIOIF 6
#define USIPF 5
#define USISIE 7
#define USISIF 7
#define USITC 0
#define USIWM0 4
#define USIWM1 5
#define WGM01 1
#define WGM12 3

// Normal Hardware Registers

extern uint8_t ADCH_history[];
extern uint8_t ADCH_sequence[];
extern uint16_t ADCH_sequence_index;
extern uint16_t ADCH_sequence_max;
extern uint16_t ADCH_index;
extern volatile uint8_t ADCH_value;

extern uint8_t ADCL_history[];
extern uint8_t ADCL_sequence[];
extern uint16_t ADCL_sequence_index;
extern uint16_t ADCL_sequence_max;
extern uint16_t ADCL_index;
extern volatile uint8_t ADCL_value;

extern uint8_t ADCSRA_history[];
extern uint8_t ADCSRA_sequence[];
extern uint16_t ADCSRA_sequence_index;
extern uint16_t ADCSRA_sequence_max;
extern uint16_t ADCSRA_index;
extern volatile uint8_t ADCSRA_value;

extern uint8_t ADMUX_history[];
extern uint8_t ADMUX_sequence[];
extern uint16_t ADMUX_sequence_index;
extern uint16_t ADMUX_sequence_max;
extern uint16_t ADMUX_index;
extern volatile uint8_t ADMUX_value;

extern uint8_t DDRB_history[];
extern uint8_t DDRB_sequence[];
extern uint16_t DDRB_sequence_index;
extern uint16_t DDRB_sequence_max;
extern uint16_t DDRB_index;
extern volatile uint8_t DDRB_value;

extern uint8_t DDRC_history[];
extern uint8_t DDRC_sequence[];
extern uint16_t DDRC_sequence_index;
extern uint16_t DDRC_sequence_max;
extern uint16_t DDRC_index;
extern volatile uint8_t DDRC_value;

extern uint8_t DDRD_history[];
extern uint8_t DDRD_sequence[];
extern uint16_t DDRD_sequence_index;
extern uint16_t DDRD_sequence_max;
extern uint16_t DDRD_index;
extern volatile uint8_t DDRD_value;

extern uint8_t GTCCR_history[];
extern uint8_t GTCCR_sequence[];
extern uint16_t GTCCR_sequence_index;
extern uint16_t GTCCR_sequence_max;
extern uint16_t GTCCR_index;
extern volatile uint8_t GTCCR_value;

extern uint8_t OCR0A_history[];
extern uint8_t OCR0A_sequence[];
extern uint16_t OCR0A_sequence_index;
extern uint16_t OCR0A_sequence_max;
extern uint16_t OCR0A_index;
extern volatile uint8_t OCR0A_value;

extern uint8_t OCR1A_history[];
extern uint8_t OCR1A_sequence[];
extern uint16_t OCR1A_sequence_index;
extern uint16_t OCR1A_sequence_max;
extern uint16_t OCR1A_index;
extern volatile uint8_t OCR1A_value;

extern uint8_t OCR1AH_history[];
extern uint8_t OCR1AH_sequence[];
extern uint16_t OCR1AH_sequence_index;
extern uint16_t OCR1AH_sequence_max;
extern uint16_t OCR1AH_index;
extern volatile uint8_t OCR1AH_value;

extern uint8_t OCR1AL_history[];
extern uint8_t OCR1AL_sequence[];
extern uint16_t OCR1AL_sequence_index;
extern uint16_t OCR1AL_sequence_max;
extern uint16_t OCR1AL_index;
extern volatile uint8_t OCR1AL_value;

extern uint8_t PINA_history[];
extern uint8_t PINA_sequence[];
extern uint16_t PINA_sequence_index;
extern uint16_t PINA_sequence_max;
extern uint16_t PINA_index;
extern volatile uint8_t PINA_value;

extern uint8_t PINB_history[];
extern uint8_t PINB_sequence[];
extern uint16_t PINB_sequence_index;
extern uint16_t PINB_sequence_max;
extern uint16_t PINB_index;
extern volatile uint8_t PINB_value;

extern uint8_t PINC_history[];
extern uint8_t PINC_sequence[];
extern uint16_t PINC_sequence_index;
extern uint16_t PINC_sequence_max;
extern uint16_t PINC_index;
extern volatile uint8_t PINC_value;

extern uint8_t PIND_history[];
extern uint8_t PIND_sequence[];
extern uint16_t PIND_sequence_index;
extern uint16_t PIND_sequence_max;
extern uint16_t PIND_index;
extern volatile uint8_t PIND_value;

extern uint8_t PLLCSR_history[];
extern uint8_t PLLCSR_sequence[];
extern uint16_t PLLCSR_sequence_index;
extern uint16_t PLLCSR_sequence_max;
extern uint16_t PLLCSR_index;
extern volatile uint8_t PLLCSR_value;

extern uint8_t PORTA_history[];
extern uint8_t PORTA_sequence[];
extern uint16_t PORTA_sequence_index;
extern uint16_t PORTA_sequence_max;
extern uint16_t PORTA_index;
extern volatile uint8_t PORTA_value;

extern uint8_t PORTB_history[];
extern uint8_t PORTB_sequence[];
extern uint16_t PORTB_sequence_index;
extern uint16_t PORTB_sequence_max;
extern uint16_t PORTB_index;
extern volatile uint8_t PORTB_value;

extern uint8_t PORTC_history[];
extern uint8_t PORTC_sequence[];
extern uint16_t PORTC_sequence_index;
extern uint16_t PORTC_sequence_max;
extern uint16_t PORTC_index;
extern volatile uint8_t PORTC_value;

extern uint8_t PORTD_history[];
extern uint8_t PORTD_sequence[];
extern uint16_t PORTD_sequence_index;
extern uint16_t PORTD_sequence_max;
extern uint16_t PORTD_index;
extern volatile uint8_t PORTD_value;

extern uint8_t SPCR_history[];
extern uint8_t SPCR_sequence[];
extern uint16_t SPCR_sequence_index;
extern uint16_t SPCR_sequence_max;
extern uint16_t SPCR_index;
extern volatile uint8_t SPCR_value;

extern uint8_t SPDR_history[];
extern uint8_t SPDR_sequence[];
extern uint16_t SPDR_sequence_index;
extern uint16_t SPDR_sequence_max;
extern uint16_t SPDR_index;
extern volatile uint8_t SPDR_value;

extern uint8_t SPSR_history[];
extern uint8_t SPSR_sequence[];
extern uint16_t SPSR_sequence_index;
extern uint16_t SPSR_sequence_max;
extern uint16_t SPSR_index;
extern volatile uint8_t SPSR_value;

extern uint8_t SREG_history[];
extern uint8_t SREG_sequence[];
extern uint16_t SREG_sequence_index;
extern uint16_t SREG_sequence_max;
extern uint16_t SREG_index;
extern volatile uint8_t SREG_value;

extern uint8_t TCCR0A_history[];
extern uint8_t TCCR0A_sequence[];
extern uint16_t TCCR0A_sequence_index;
extern uint16_t TCCR0A_sequence_max;
extern uint16_t TCCR0A_index;
extern volatile uint8_t TCCR0A_value;

extern uint8_t TCCR0B_history[];
extern uint8_t TCCR0B_sequence[];
extern uint16_t TCCR0B_sequence_index;
extern uint16_t TCCR0B_sequence_max;
extern uint16_t TCCR0B_index;
extern volatile uint8_t TCCR0B_value;

extern uint8_t TCCR1_history[];
extern uint8_t TCCR1_sequence[];
extern uint16_t TCCR1_sequence_index;
extern uint16_t TCCR1_sequence_max;
extern uint16_t TCCR1_index;
extern volatile uint8_t TCCR1_value;

extern uint8_t TCCR1A_history[];
extern uint8_t TCCR1A_sequence[];
extern uint16_t TCCR1A_sequence_index;
extern uint16_t TCCR1A_sequence_max;
extern uint16_t TCCR1A_index;
extern volatile uint8_t TCCR1A_value;

extern uint8_t TCCR1B_history[];
extern uint8_t TCCR1B_sequence[];
extern uint16_t TCCR1B_sequence_index;
extern uint16_t TCCR1B_sequence_max;
extern uint16_t TCCR1B_index;
extern volatile uint8_t TCCR1B_value;

extern uint8_t TCNT0_history[];
extern uint8_t TCNT0_sequence[];
extern uint16_t TCNT0_sequence_index;
extern uint16_t TCNT0_sequence_max;
extern uint16_t TCNT0_index;
extern volatile uint8_t TCNT0_value;

extern uint8_t TCNT1_history[];
extern uint8_t TCNT1_sequence[];
extern uint16_t TCNT1_sequence_index;
extern uint16_t TCNT1_sequence_max;
extern uint16_t TCNT1_index;
extern volatile uint8_t TCNT1_value;

extern uint8_t TCNT1H_history[];
extern uint8_t TCNT1H_sequence[];
extern uint16_t TCNT1H_sequence_index;
extern uint16_t TCNT1H_sequence_max;
extern uint16_t TCNT1H_index;
extern volatile uint8_t TCNT1H_value;

extern uint8_t TCNT1L_history[];
extern uint8_t TCNT1L_sequence[];
extern uint16_t TCNT1L_sequence_index;
extern uint16_t TCNT1L_sequence_max;
extern uint16_t TCNT1L_index;
extern volatile uint8_t TCNT1L_value;

extern uint8_t TIFR0_history[];
extern uint8_t TIFR0_sequence[];
extern uint16_t TIFR0_sequence_index;
extern uint16_t TIFR0_sequence_max;
extern uint16_t TIFR0_index;
extern volatile uint8_t TIFR0_value;

extern uint8_t TIMSK_history[];
extern uint8_t TIMSK_sequence[];
extern uint16_t TIMSK_sequence_index;
extern uint16_t TIMSK_sequence_max;
extern uint16_t TIMSK_index;
extern volatile uint8_t TIMSK_value;

extern uint8_t TIMSK0_history[];
extern uint8_t TIMSK0_sequence[];
extern uint16_t TIMSK0_sequence_index;
extern uint16_t TIMSK0_sequence_max;
extern uint16_t TIMSK0_index;
extern volatile uint8_t TIMSK0_value;

extern uint8_t TIMSK1_history[];
extern uint8_t TIMSK1_sequence[];
extern uint16_t TIMSK1_sequence_index;
extern uint16_t TIMSK1_sequence_max;
extern uint16_t TIMSK1_index;
extern volatile uint8_t TIMSK1_value;

extern uint8_t TWBR_history[];
extern uint8_t TWBR_sequence[];
extern uint16_t TWBR_sequence_index;
extern uint16_t TWBR_sequence_max;
extern uint16_t TWBR_index;
extern volatile uint8_t TWBR_value;

extern uint8_t TWCR_history[];
extern uint8_t TWCR_sequence[];
extern uint16_t TWCR_sequence_index;
extern uint16_t TWCR_sequence_max;
extern uint16_t TWCR_index;
extern volatile uint8_t TWCR_value;

extern uint8_t TWSR_history[];
extern uint8_t TWSR_sequence[];
extern uint16_t TWSR_sequence_index;
extern uint16_t TWSR_sequence_max;
extern uint16_t TWSR_index;
extern volatile uint8_t TWSR_value;

extern uint8_t UBRR0H_history[];
extern uint8_t UBRR0H_sequence[];
extern uint16_t UBRR0H_sequence_index;
extern uint16_t UBRR0H_sequence_max;
extern uint16_t UBRR0H_index;
extern volatile uint8_t UBRR0H_value;

extern uint8_t UBRR0L_history[];
extern uint8_t UBRR0L_sequence[];
extern uint16_t UBRR0L_sequence_index;
extern uint16_t UBRR0L_sequence_max;
extern uint16_t UBRR0L_index;
extern volatile uint8_t UBRR0L_value;

extern uint8_t UCSR0A_history[];
extern uint8_t UCSR0A_sequence[];
extern uint16_t UCSR0A_sequence_index;
extern uint16_t UCSR0A_sequence_max;
extern uint16_t UCSR0A_index;
extern volatile uint8_t UCSR0A_value;

extern uint8_t UCSR0B_history[];
extern uint8_t UCSR0B_sequence[];
extern uint16_t UCSR0B_sequence_index;
extern uint16_t UCSR0B_sequence_max;
extern uint16_t UCSR0B_index;
extern volatile uint8_t UCSR0B_value;

extern uint8_t UCSR0C_history[];
extern uint8_t UCSR0C_sequence[];
extern uint16_t UCSR0C_sequence_index;
extern uint16_t UCSR0C_sequence_max;
extern uint16_t UCSR0C_index;
extern volatile uint8_t UCSR0C_value;

extern uint8_t UDR0_history[];
extern uint8_t UDR0_sequence[];
extern uint16_t UDR0_sequence_index;
extern uint16_t UDR0_sequence_max;
extern uint16_t UDR0_index;
extern volatile uint8_t UDR0_value;

extern uint8_t USICR_history[];
extern uint8_t USICR_sequence[];
extern uint16_t USICR_sequence_index;
extern uint16_t USICR_sequence_max;
extern uint16_t USICR_index;
extern volatile uint8_t USICR_value;

extern uint8_t USIDR_history[];
extern uint8_t USIDR_sequence[];
extern uint16_t USIDR_sequence_index;
extern uint16_t USIDR_sequence_max;
extern uint16_t USIDR_index;
extern volatile uint8_t USIDR_value;

extern uint8_t USISR_history[];
extern uint8_t USISR_sequence[];
extern uint16_t USISR_sequence_index;
extern uint16_t USISR_sequence_max;
extern uint16_t USISR_index;
extern volatile uint8_t USISR_value;

extern uint8_t TWDR_history[];
extern uint8_t TWDR_sequence[];
extern uint16_t TWDR_sequence_index;
extern uint16_t TWDR_sequence_max;
extern uint16_t TWDR_index;
extern volatile uint8_t TWDR_value;


#endif  // AVR_IO_COMMON_H
