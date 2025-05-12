#include "spi.h"
#include <avr/io.h>

#define SPI_DDR_PORT DDRB
#define USCK_DD_PIN 2
#define DO_DD_PIN 1
#define DI_DD_PIN 0

void spi_initMaster(uint8_t flags) {
  USICR = 
      (0 << USISIE) |   // Disable Start condition interrupt
      (0 << USIOIE) |   // Disable the counter overflow interrupt
      (0 << USIWM1) | (1 << USIWM0) |   // USIWMX -> 01 for 3-wire mode
      (1 << USICS1) | (0 << USICS0) | (1 << USICLK) |  // USITC clock strobe
      (0 << USITC);  // No strobe yet

      SPI_DDR_PORT |= (1 << USCK_DD_PIN);  // Make SCLK an output

      if (flags & SPI_INVERT_CLOCK) {
        USICR |= (1 << USICS0);
      }

      if (flags & SPI_USE_MOSI) {
        SPI_DDR_PORT |= (1 << DO_DD_PIN);
      }

      if (flags & SPI_USE_MISO) {
        SPI_DDR_PORT &= ~(1 << DI_DD_PIN);
      }
}

void spi_initMasterFreq(uint8_t flags, uint32_t freq) {
  spi_initMaster(flags);
}

static void genClock(void) {
  const uint8_t bit_high =
    (USICR | (1 << USITC)) & ~(1 << USICLK);  // Clock out
  const uint8_t bit_low =
    (USICR | (1 << USITC)) | (1 << USICLK);  // Inverted Clock out

  USISR = (1 << USIOIF);  // clear counter and counter overflow

  USICR = bit_high;  // bit 7
  USICR = bit_low;

  USICR = bit_high;  // bit 6
  USICR = bit_low;

  USICR = bit_high;  // bit 5
  USICR = bit_low;

  USICR = bit_high;  // bit 4
  USICR = bit_low;

  USICR = bit_high;  // bit 3
  USICR = bit_low;

  USICR = bit_high;  // bit 2
  USICR = bit_low;

  USICR = bit_high;  // bit 1
  USICR = bit_low;

  USICR = bit_high;  // bit 0
  USICR = bit_low;
}

void spi_syncWriteBlock(const uint8_t* data, uint16_t len) {
  for (uint16_t i=0; i<len; ++i) {
    spi_syncWrite(data[i]);
  }
}

void spi_syncWrite(uint8_t data) {
  USIDR = data;
  genClock();
}

uint8_t spi_syncRead() {
  genClock();
  return USIDR;
}

uint8_t spi_syncTransact(uint8_t data) {
  USIDR = data;
  genClock();
  return USIDR;
}
