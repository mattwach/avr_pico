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
#define BRIGHTNESS 5   // 0-31
#define SS (1 << 2)  // port

uint32_t colors[10] = {
  0x000000, // 0 - black
  0xF00000, // 1 - red
  0xFF8000, // 2 - orange
  0xFFFF00, // 3- yellow
  0x00FF00, // 4 - green
  0x0000FF, // 5 - blue
  0x4B0082, // 6 - indigo
  0x8F20FF, // 7 - violet
  0x080810, // 8 - dark blue
  0xFFFFFF, // 9 - white
};

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

static inline void colorIndex(uint8_t index) {
  const uint32_t color = colors[index]; 
  writeBytes(0xE0 | BRIGHTNESS, color >> 16, color & 0xFF, (color >> 8) & 0xFF);
}

int main() {
  DDRB |= SS;
  PORTB |= SS;
  spi_initMasterFreq(SPI_USE_MOSI, SPI_SPEED);
  uint16_t i = 0;
  _delay_ms(50);
  while (1) {
    startFrame();
    colorIndex(i / 100);
    colorIndex((i / 10) % 10);
    colorIndex(i % 10);
    endFrame();
    ++i;
    if (i >= 1000) {
      i = 0;
    }
    _delay_ms(500);
  }
}
