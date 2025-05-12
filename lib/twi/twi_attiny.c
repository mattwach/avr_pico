#include "twi.h"
#include <avr/io.h>
#include <util/delay.h>


#if defined(__AVR_ATtiny84__) | defined(__AVR_ATtiny44__)
#define DDR_USI DDRA
#define PORT_USI PORTA
#define PIN_USI PINA
#define PORT_USI_SDA PORTA6
#define PORT_USI_SCL PORTA4
#define PIN_USI_SDA PINA6
#define PIN_USI_SCL PINA4
#endif

#if defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__)
#define DDR_USI DDRB
#define PORT_USI PORTB
#define PIN_USI PINB
#define PORT_USI_SDA PORTB5
#define PORT_USI_SCL PORTB7
#define PIN_USI_SDA PINB5
#define PIN_USI_SCL PINB7
#endif

// Default catch-all that will work for ATTiny84, 85, etc but obviously not
// everyone In any case, something like a mega wont get very far in this file
// before blowing up so it not an important practical point
#ifndef PIN_USI
#define DDR_USI DDRB
#define PORT_USI PORTB
#define PIN_USI PINB
#define PORT_USI_SDA PORTB0
#define PORT_USI_SCL PORTB2
#define PIN_USI_SDA PINB0
#define PIN_USI_SCL PINB2
#endif


// Defines controlling timing limits - SCL <= 100KHz.

// For use with _delay_us()
#if F_CPU == 1000000
#define T2_TWI 2
#define T4_TWI 1
#else
// just using the defaults for the other mhz cases as I would need to verify them
#define T2_TWI 2 
#define T4_TWI 1
#endif

// Bit position for LSB of the slave address bits in the init byte.
#define TWI_ADR_BITS 1
// Bit position for (N)ACK bit.
#define TWI_NACK_BIT 0
// indicates sending to TWI
#define USI_SEND 0
// indicates receiving from TWI
#define USI_RCVE 1
// Prepare register value to: Clear flags, and set USI to shift 8 bits i.e. count 16 clock edges.
#define USISR_8BIT ((1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC))
// Prepare register value to: Clear flags, and set USI to shift 1 bit i.e. count 2 clock edges.
#define USISR_1BIT ((1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | (0xE << USICNT0))


/*---------------------------------------------------------------
  Core function for shifting data in and out from the USI.
  Data to be sent has to be placed into the USIDR prior to calling
  this function. Data read, will be return'ed from the function.
  ---------------------------------------------------------------*/
static uint8_t USI_ShiftData(uint8_t temp) {
  USISR = temp;  // Set USISR according to temp.
  // Prepare clocking.
  temp = (0 << USISIE) | (0 << USIOIE) |                  // Interrupts disabled
         (1 << USIWM1) | (0 << USIWM0) |                  // Set USI in Two-wire mode.
         (1 << USICS1) | (0 << USICS0) | (1 << USICLK) |  // Software clock strobe as source.
         (1 << USITC);                                    // Toggle Clock Port.
  do {
    _delay_us(T2_TWI);
    USICR = temp;  // Generate positve SCL edge.
    while (!(PIN_USI & (1 << PIN_USI_SCL)))
      ;  // Wait for SCL to go high.
    _delay_us(T4_TWI);
    USICR = temp;                      // Generate negative SCL edge.
  } while (!(USISR & (1 << USIOIF)));  // Check for transfer complete.

  _delay_us(T2_TWI);
  temp = USIDR;                   // Read out data.
  USIDR = 0xFF;                   // Release SDA.
  DDR_USI |= (1 << PIN_USI_SDA);  // Enable SDA as output.

  return temp;  // Return the data from the USIDR
}


static error_t USI_Start(void) {
  /* Release SCL to ensure that (repeated) Start can be performed */
  PORT_USI |= (1 << PIN_USI_SCL);  // Release SCL.
  while (!(PIN_USI & (1 << PIN_USI_SCL)))
    ;  // Verify that SCL becomes high.
  _delay_us(T2_TWI);

  /* Generate Start Condition */
  PORT_USI &= ~(1 << PIN_USI_SDA);  // Force SDA LOW.
  _delay_us(T4_TWI);
  PORT_USI &= ~(1 << PIN_USI_SCL);  // Pull SCL LOW.
  PORT_USI |= (1 << PIN_USI_SDA);   // Release SDA.

  if (!(USISR & (1 << USISIF))) {
    return TWI_MISSING_START_CON_ERROR;
  }
  return 0;
}

static error_t USI_WriteByte(uint8_t byte) {
  PORT_USI &= ~(1 << PIN_USI_SCL);      // Pull SCL LOW.
  USIDR = byte;                         // Setup data.
  USI_ShiftData(USISR_8BIT);  // Send 8 bits on bus.

  /* Clock and verify (N)ACK from slave */
  DDR_USI &= ~(1 << PIN_USI_SDA);  // Enable SDA as input.
  if (USI_ShiftData(USISR_1BIT) & (1 << TWI_NACK_BIT)) {
    return TWI_NO_ACK_ERROR;
  }

  return 0;
}

void twi_init(void) {
  static uint8_t already_initialized = 0;
  if (!already_initialized) {
    already_initialized = 1;
    twi_reinit();
  }
}

void twi_reinit(void) {
  PORT_USI |= (1 << PIN_USI_SDA);  // Enable pullup on SDA, to set high as released state.
  PORT_USI |= (1 << PIN_USI_SCL);  // Enable pullup on SCL, to set high as released state.

  DDR_USI |= (1 << PIN_USI_SCL);  // Enable SCL as output.
  DDR_USI |= (1 << PIN_USI_SDA);  // Enable SDA as output.

  USIDR = 0xFF;                                            // Preload dataregister with "released level" data.
  USICR = (0 << USISIE) | (0 << USIOIE) |                  // Disable Interrupts.
          (1 << USIWM1) | (0 << USIWM0) |                  // Set USI in Two-wire mode.
          (1 << USICS1) | (0 << USICS0) | (1 << USICLK) |  // Software stobe as counter clock source
          (0 << USITC);
  USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) |  // Clear flags,
          (0x0 << USICNT0);                                              // and reset counter.
}

void twi_stop(error_t* err) {
  if (*err) {
    return;
  }
  PORT_USI &= ~(1 << PIN_USI_SDA);  // Pull SDA low.
  PORT_USI |= (1 << PIN_USI_SCL);   // Release SCL.
  while (!(PIN_USI & (1 << PIN_USI_SCL)))
    ;  // Wait for SCL to go high.
  _delay_us(T4_TWI);
  PORT_USI |= (1 << PIN_USI_SDA);  // Release SDA.
  _delay_us(T2_TWI);

  if (!(USISR & (1 << USIPF))) {
    *err = TWI_MISSING_STOP_CON_ERROR;
  }
}


void twi_startWrite(uint8_t address, error_t* err) {
  if (*err) {
    return;
  }

  *err = USI_Start();

  if (!(*err)) {
    const uint8_t addr_byte = (address << TWI_ADR_BITS) | USI_SEND;
    *err = USI_WriteByte(addr_byte);
  }
}

void twi_writeNoStop(uint8_t byte, error_t* err) {
  if (*err) {
    return;
  }

  *err = USI_WriteByte(byte);
}

void twi_readNoStop(uint8_t address, uint8_t* data, uint8_t length, error_t* err) {
  if (*err) {
    return;
  }

  if (((*err) = USI_Start()) != 0) {
    return;
  }

  const uint8_t addr_byte = (address << TWI_ADR_BITS) | USI_RCVE;
  if (((*err = USI_WriteByte(addr_byte))) != 0) {
    return;
  }

  uint8_t i = 0;
  for (; i < length; ++i) {
    DDR_USI &= ~(1 << PIN_USI_SDA);  // Enable SDA as input.
    data[i] = USI_ShiftData(USISR_8BIT);

    // ACK the bytes up to the last one, which is NACKED.
    USIDR = (i < (length - 1)) ? 0x00 : 0xFF;
    USI_ShiftData(USISR_1BIT);
  }
}
