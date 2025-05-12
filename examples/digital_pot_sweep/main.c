// Simple demonstration app from the MCP4231 Digital POT 128 step potentiometer
//
// Steps POW through the possible range, via the given step count
//
// Arduino Wiring
//
//                 +-------+
// SS   10 ->  CS -|1    14|- VCC  <-    5V
// SCK  13 -> SCK -|2    13|- SDO  <- 12 MISO
// MOSI 11 -> SDI -|3    12|- SHDN       NC
// GND     -> VSS -|4    11|- WP         NC
// GND        P1B -|5    10|- P0B        GND
// N/C        P1W -|6     9|- P0W        OSC PROBE
// 5V         P1A -|7     8|- P1A        5V
//                 +-------+

#include <spi/spi.h>
#include <util/delay.h>

#ifdef __AVR_MEGA__
#define SS (1 << 2)  // port
#else
// ATTiny85
#define SS (1 << 4)
#endif
#define SPI_SPEED 10000000

#define STEP 0x1
#define STEP_DELAY_US 10 
#define POT_MAX 128

// 16 bit to lessen overflows
uint16_t pot_val;
int16_t pot_inc;

static inline void writePot(const uint8_t pot_number, const uint8_t val) {
  // Build up the command packet
  //
  //      MEM  CMD DATA
  // pot0 0000 00  00   ->  0x00
  // pot1 0001 00  00   ->  0x80
  const uint8_t command = pot_number > 0 ? 0x80 : 0x00;

  PORTB &= ~SS;
  _delay_us(1);
  spi_syncWrite(command);
  spi_syncWrite(val);
  _delay_us(1);
  PORTB |= SS;
}

static inline void increment() {
  pot_val += pot_inc;
  if ((pot_val > POT_MAX) || (pot_val < 0)) {
    pot_inc = -pot_inc;
    pot_val += pot_inc;
  }
}


int main() {
  pot_val = 0;
  pot_inc = STEP;
  DDRB |= SS;
  PORTB |= SS;
  spi_initMasterFreq(SPI_USE_MOSI, 1000000);
  while (1) {
    writePot(0, (uint8_t)pot_val);
    increment();
				_delay_us(STEP_DELAY_US);
  }
}
