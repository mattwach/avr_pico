// Simple demonstration app from the MCP4231 Digital POT 128 step potentiometer
//
// Steps POW through the possible range, via the given step count
//
// Arduino Wiring
//
//                 +-------+
// NC   10 ->  CS -|1    14|- VCC  <-    5V
// SCK  13 -> SCK -|2    13|- SDO  <- 12 NC
// MOSI 11 -> SDI -|3    12|- SHDN       NC
// GND     -> VSS -|4    11|- WP         NC
// GND        P1B -|5    10|- P0B        GND
// N/C        P1W -|6     9|- P0W        OSC PROBE
// 5V         P1A -|7     8|- P1A        5V
//                 +-------+

#include <spi/spi.h>
#include <util/delay.h>

#define SPI_SPEED 1000000
#define BRIGHTNESS 10   // 0-31
#define LED_PIN 5  // D5

#define SS (1 << 2)  // port

static inline void writeBytes(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
  PORTB &= ~SS;
  _delay_us(1);
  spi_syncWrite(b1);
  spi_syncWrite(b2);
  spi_syncWrite(b3);
  spi_syncWrite(b4);
  _delay_us(1);
  PORTB &= ~SS;
}

static inline void startFrame() {
  writeBytes(0, 0, 0, 0);
}

static inline void endFrame() {
  writeBytes(0xFF, 0xFF, 0xFF, 0xFF);
}

static inline void ledFrame(uint8_t red, uint8_t green, uint8_t blue) {
  writeBytes(0xE0 | BRIGHTNESS, red, blue, green);
}

int main() {
  DDRD = 1 << LED_PIN;
  DDRB |= SS;
  PORTB |= SS;
  spi_initMasterFreq(SPI_USE_MOSI, SPI_SPEED);
  while (1) {
    PORTD |= 1 << LED_PIN;
    _delay_ms(200);
    PORTD &= ~(1 << LED_PIN);
    startFrame();
    ledFrame(0xFF, 0x00, 0x00);  // red
    ledFrame(0x00, 0xFF, 0x00);  // green
    ledFrame(0xFF, 0xFF, 0xFF);  // white
    endFrame();
    _delay_ms(800);
  }
}
