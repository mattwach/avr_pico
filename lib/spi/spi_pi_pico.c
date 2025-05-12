#include "spi.h"
#include <pico/stdlib.h>
#include <hardware/spi.h>

// Consider using the library function directly, esp if you are reading/writing
// blocks of data.

#define SCK_PIN 18
#define MOSI_PIN 19
#define MISO_PIN 16

// spi_initMaster is missing on purpose

void spi_initMasterFreq(uint8_t flags, uint32_t freq) {
  // Initialize SPI port
  spi_init(spi0, freq);

  // Set SPI format
  spi_set_format( spi0,   // SPI instance
      8,      // Number of bits per transfer
      (flags & SPI_INVERT_CLOCK) ? 0 : 1,      // Polarity (CPOL)
      (flags & SPI_PHASE) ? 0 : 1,      // Phase (CPHA)
      (flags & SPI_LSB) ? SPI_LSB_FIRST : SPI_MSB_FIRST);

  // Initialize SPI pins
  gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
  if (flags & SPI_USE_MOSI) {
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
  }
  if (flags & SPI_USE_MISO) {
    gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
  }
}

void spi_syncWrite(uint8_t data) {
  spi_write_blocking(spi0, &data, 1);
}

void spi_syncWriteBlock(const uint8_t* data, uint16_t len) {
  spi_write_blocking(spi0, data, len);
}

uint8_t spi_syncTransact(uint8_t data) {
  uint8_t buff;
  spi_write_blocking(spi0, &data, 1);
  spi_read_blocking(spi0, 1, &buff, 1);
  return buff;
}

uint8_t spi_syncRead() {
  uint8_t buff;
  spi_read_blocking(spi0, 1, &buff, 1);
  return buff;
}
