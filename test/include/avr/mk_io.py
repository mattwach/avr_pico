# generates io.h and io.c

MACROS = (
    '#define ADC ((ADCH) << 8 | (ADCL))',
)

NORMAL_REGISTERS = (
    ('ADCH', '0'),    # ADC Result High
    ('ADCL', '0'),    # ADC Result low
    ('ADCSRA', '0'),  # ADC Status register
    ('ADMUX', '0'),   # ADC Mux register
    ('DDRB', '0'),    # PORTB Data direction
    ('DDRC', '0'),    # PORTC Data direction
    ('DDRD', '0'),    # PORTD Data direction
    ('GTCCR', '0'),   # General Timer/Counter1 Control Register
    ('OCR0A', '0'),   # Output Compare Register 0 A
    ('OCR1A', '0'),   # Output Compare Register 1 A
    ('OCR1AH', '0'),  # Output Compare Register 1 A High
    ('OCR1AL', '0'),  # Output Compare Register 1 A Low
    ('PINA', '0'),    # PORTA State
    ('PINB', '0'),    # PORTB State
    ('PINC', '0'),    # PORTC State
    ('PIND', '0'),    # PORTD State
    ('PLLCSR', '0'),  # PLL Control and Status Registe
    ('PORTA', '0'),   # PORTB State
    ('PORTB', '0'),   # PORTB State
    ('PORTC', '0'),   # PORTC State
    ('PORTD', '0'),   # PORTD State
    ('SPCR', '0'),    # SPI Control register
    ('SPDR', '0'),    # SPI Data register
    ('SPSR', '0'),    # SPI Status register
    ('SREG', '0'),    # Main status register
    ('TCCR0A', '0'),  # Timer/Counter0 Control Register A
    ('TCCR0B', '0'),  # Timer/Counter0 Control Register B
    ('TCCR1', '0'),   # Timer/Counter1 Control Register
    ('TCCR1A', '0'),  # Timer/Counter1 Control Register A
    ('TCCR1B', '0'),  # Timer/Counter1 Control Register B
    ('TCNT0', '0'),   # Timer counter 0
    ('TCNT1', '0'),   # Timer counter 1
    ('TCNT1H', '0'),  # Timer counter 1 High
    ('TCNT1L', '0'),  # Timer counter 1 Low
    ('TIFR0', '0'),   # Timer/Counter Interrupt Flag Register
    ('TIMSK', '0'),   # Timer/Counter Interrupt Mask Register
    ('TIMSK0', '0'),  # Timer/Counter0 Interrupt Mask Register
    ('TIMSK1', '0'),  # Timer/Counter1 Interrupt Mask Register
    ('TWBR', '0'),    # TWI bit rate register
    ('TWCR', '0'),    # TWI control register
    ('TWSR', '0'),    # TWI status register
    ('UBRR0H', '0'),  # USART Bit rate High
    ('UBRR0L', '0'),  # USART Bit rate low
    ('UCSR0A', '(1 << UDRE0)'),  # USART Status A
    ('UCSR0B', '0'),  # USART Status B
    ('UCSR0C', '0'),  # USART Status C
    ('UDR0', '0'),    # UART Data senv/recv
    ('USICR', '0'),   # USI Control register
    ('USIDR', '0'),   # USI Data register (Does this need an interrupt?)
    ('USISR', '0'),   # USI Status register
)

INTERRUPT_REGISTERS = (
    ('TWDR', 0, 'TWCR_value & (1<<TWINT)', 'TWI_TX_vect'),  # UART Data senv/recv
)

BITS = (
    ('ADEN', 7),    # ADCSRA ADC Enable
    ('ADIE', 3),    # ADCSRA Interrput enable
    ('ADIF', 4),    # ADCSRA Interrput flag
    ('ADSC', 6),    # ADCSRA ADC Start Conversion
    ('ADLAR', 5),   # ADMUX ADC LEft Adjust result
    ('CPHA', 2),    # SPCR Clock phase
    ('CPOL', 3),    # SPCR Clock polarity
    ('DORD', 5),    # SPCR Data Order
    ('MSTR', 4),    # SPCR Master/Slave select
    ('OCIE0A', 4),  # TIMSK1: Timer/Counter0, Output Compare A Match Interrupt Enable
    ('OCIE1A', 1),  # TIMSK1: Timer/Counter1, Output Compare A Match Interrupt Enable
    ('PINB0', 0),   # PINB Bit
    ('PINB1', 1),   # PINB Bit
    ('PINB2', 2),   # PINB Bit
    ('PINB3', 3),   # PINB Bit
    ('PINB4', 4),   # PINB Bit
    ('PINB5', 5),   # PINB Bit
    ('PINB6', 6),   # PINB Bit
    ('PINB7', 7),   # PINB Bit
    ('PORTB0', 0),  # PORTB Bit
    ('PORTB1', 1),  # PORTB Bit
    ('PORTB2', 2),  # PORTB Bit
    ('PORTB3', 3),  # PORTB Bit
    ('PORTB4', 4),  # PORTB Bit
    ('PORTB5', 5),  # PORTB Bit
    ('PORTB6', 6),  # PORTB Bit
    ('PORTB7', 7),  # PORTB Bit
    ('RXC0', 7),    # USART Receive complete
    ('RXCIE0', 7),  # Enable USART RX interrupts
    ('RXEN0', 4),   # Enable USART RX
    ('SPE', 6),     # SPCR SPI Enable
    ('SPI2X', 0),   # SPSR Double SPI Speed bit
    ('SPIF', 7),    # SPSR SPI Interrupt flag
    ('SPR0', 0),    # SPCR SPI Clock Rate Select 0
    ('SPR1', 1),    # SPCR SPI Clock Rate Select 1
    ('TWEA', 6),    # TWCR Enable Ack
    ('TWEN', 2),    # TWCR Start condition bit
    ('TWINT', 7),   # TWCR interrupt flag
    ('TWPS0', 0),   # TWSR prescaler 0
    ('TWPS1', 1),   # TWSR prescaler 1
    ('TWSTA', 5),   # TWCR Start condition bit
    ('TWSTO', 4),   # TWCR Stop condition bit
    ('TXCIE0', 6),  # Enable USART TX interrupts
    ('TXEN0', 3),   # Enable USART TX
    ('U2X0', 1),    # Set USART Duble rate mode
    ('UCSZ00', 1),  # USART Bit size 0
    ('UCSZ01', 2),  # USART Bit size 1
    ('UDRE0', 5),   # USART UDRx available
    ('USICLK', 1),  # USICR: Clock Strobe
    ('USICNT0', 0), # USISR: Counter Value
    ('USICNT1', 1), # USISR: Counter Value
    ('USICNT2', 2), # USISR: Counter Value
    ('USICNT3', 3), # USISR: Counter Value
    ('USICS0', 2),  # USICR: Clock source select (bit 0 of 1)
    ('USICS1', 3),  # USICR: Clock source select (bit 1 of 1)
    ('USIDC', 4),   # USISR: Data Output Collision
    ('USIOIE', 6),  # USICR: Counter Overflow Interrupt Enable
    ('USIOIF', 6),  # USISR: Counter Overflow Interrupt Flag
    ('USIPF', 5),   # USISR: Stop Condition Flag
    ('USISIE', 7),  # USICR: Start Condition Interrupt Enable
    ('USISIF', 7),  # USISR: Start Condition Interrupt Flag
    ('USITC', 0),   # USICR: Toggle Clock Port Pin
    ('USIWM0', 4),  # USICR: Wire Mode (Bit 0 of 1)
    ('USIWM1', 5),  # USICR: Wire Mode (Bit 1 of 1)
    ('WGM01', 1),   # TCCR0A: Waveform Generation Mode
    ('WGM12', 3),   # TCCR1B: Waveform Generation Mode
)

ALL_REGISTERS = list(NORMAL_REGISTERS)
ALL_REGISTERS.extend((r[0], r[1]) for r in INTERRUPT_REGISTERS)

with open('io_common.h', 'w') as f:
    f.write(
        '\n'.join((
             '#ifndef AVR_IO_COMMON_H',
             '#define AVR_IO_COMMON_H',
             '',
             '#include <inttypes.h>',
             '',
        ))
    )

    f.write('\n\n// Bit Definitions\n\n')

    for name, val in BITS:
        f.write('#define {name} {val}\n'.format(name=name, val=val))

    f.write('\n// Normal Hardware Registers\n\n')

    for name, _ in ALL_REGISTERS:
        f.write(
            '\n'.join((
                'extern uint8_t {name}_history[];',
                'extern uint8_t {name}_sequence[];',
                'extern uint16_t {name}_sequence_index;',
                'extern uint16_t {name}_sequence_max;',
                'extern uint16_t {name}_index;',
                'extern volatile uint8_t {name}_value;\n\n',
            )).format(name=name)
        )

    f.write('\n#endif  // AVR_IO_COMMON_H\n')

with open('io.h', 'w') as f:
    f.write(
        '\n'.join((
             '#ifndef AVR_IO_H',
             '#define AVR_IO_H',
             '',
             '#include "io_common.h"',
             '',
             '// Useful defines',
             '#define ISR(fn) void fn(void)',
             ''
        ))
    )

    f.write('\n\n// Normal Hardware Registers\n\n')

    for name, _ in NORMAL_REGISTERS:
        f.write(
            '\n'.join((
                'extern uint8_t* {name}_();',
                '#define {name} *({name}_())\n\n'
            )).format(name=name)
        )

    f.write('// Interrupt Triggering Hardware Registers\n\n')

    for name, _, test, vector_fn in INTERRUPT_REGISTERS:
        f.write(
            '\n'.join((
                '#define {name} "Do not access {name} directly, please define a GET_{name}, SET_{name} (test/Readme.md)"',
                'extern uint8_t* {name}_();',
                '#ifdef USE_{vector_fn}',
                'void {vector_fn}(void);',
                '#define SET_{name}(v) *({name}_()) = (v); if ((SREG_value & 0x80) && ({test})) {{ {vector_fn}(); }}',
                '#else',
                '#define SET_{name}(v) *({name}_()) = (v)',
                '#endif',
                '#define GET_{name}() *({name}_())',
                '\n',
            )).format(name=name, test=test, vector_fn=vector_fn)
        )

    f.write('\n\n// Macros\n\n')
    f.write('\n'.join(MACROS))

    f.write('\n\n#endif  // AVR_IO_H\n')

with open('io.c', 'w') as f:
    f.write(
        '\n'.join((
             '#include "io_common.h"',
             '#include <inttypes.h>',
             '#include <string.h>',
        ))
    )

    f.write('\n\n')

    for name, _ in ALL_REGISTERS:
        f.write(
            '\n'.join((
                'uint8_t {name}_history[1024];',
                'uint8_t {name}_sequence[1024];',
                'uint16_t {name}_sequence_index;',
                'uint16_t {name}_sequence_max;',
                'uint16_t {name}_index;',
                'volatile uint8_t {name}_value;',
                'volatile uint8_t* {name}_() {{',
                '  {name}_history[{name}_index++] = {name}_value;',
                '  if ({name}_sequence_index < {name}_sequence_max) {{',
                '    {name}_value = {name}_sequence[{name}_sequence_index++];',
                '  }}',
                '  return &{name}_value;',
                '}}\n\n',
            )).format(name=name)
        )

    f.write('const unsigned int KNOWN_REGISTER_COUNT = %d;\n' % len(ALL_REGISTERS))
    f.write('uint16_t* const KNOWN_REGISTER_INDEX[] = {\n')
    for name, _ in ALL_REGISTERS:
        f.write('  &{name}_index,\n'.format(name=name))
    f.write('};\n\n')

    f.write('const char* KNOWN_REGISTER_NAME[] = {\n')
    for name, _ in ALL_REGISTERS:
        f.write('  "{name}",\n'.format(name=name))
    f.write('};\n\n')



    f.write('void avr_reset(void) {\n')
    for name, val in ALL_REGISTERS:
        f.write(
            '\n'.join((
                '  {name}_value = {val};',
                '  {name}_index = 0;',
                '  {name}_sequence_index = 0;',
                '  {name}_sequence_max = 0;',
                '  memset({name}_history, 0, sizeof({name}_history));\n\n',
            )).format(name=name, val=val)
        )
    f.write('}\n')
