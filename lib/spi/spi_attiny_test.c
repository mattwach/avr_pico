#include "spi.h"

#include <test/unit_test.h>

void test_init_mosi(void) {
  spi_initMasterFreq(SPI_USE_MOSI, 1000000);
  assert_reg_equal(0x1A, USICR);
  assert_reg_equal(0x06, DDRB);
}

void test_init_miso(void) {
  sequence(DDRB, 0x01); // Preset bit to make sure it's cleared
  spi_initMasterFreq(SPI_USE_MISO, 1000000);
  assert_reg_equal(0x1A, USICR);
  assert_reg_equal(0x04, DDRB);
}

void test_init_invert(void) {
  spi_initMasterFreq(SPI_INVERT_CLOCK, 1000000);
  assert_reg_equal(0x1E, USICR);
  assert_reg_equal(0x04, DDRB);
}

void test_syncWrite(void) {
    spi_initMaster(SPI_USE_MOSI);
    reset_history();

    spi_syncWrite(0xAB);
    assert_history(USIDR, 0xAB);
    assert_history(USISR, 0x40);
    assert_history(USICR,
        0x1A, 0x1A,  // Reading twice to set regs
        0x19, 0x1B,  // Bit 7 strobe
        0x19, 0x1B,  // Bit 6 strobe
        0x19, 0x1B,  // Bit 5 strobe
        0x19, 0x1B,  // Bit 4 strobe
        0x19, 0x1B,  // Bit 3 strobe
        0x19, 0x1B,  // Bit 2 strobe
        0x19, 0x1B,  // Bit 1 strobe
        0x19, 0x1B,  // Bit 0 strobe
    );
}

void test_syncRead(void) {
    spi_initMaster(SPI_USE_MISO);
    reset_history();
    sequence(USIDR, 0xAB);

    uint8_t val = spi_syncRead();
    assert_int_equal(0xAB, val);
    assert_history(USISR, 0x40);
    assert_history(USICR,
        0x1A, 0x1A,  // Reading twice to set regs
        0x19, 0x1B,  // Bit 7 strobe
        0x19, 0x1B,  // Bit 6 strobe
        0x19, 0x1B,  // Bit 5 strobe
        0x19, 0x1B,  // Bit 4 strobe
        0x19, 0x1B,  // Bit 3 strobe
        0x19, 0x1B,  // Bit 2 strobe
        0x19, 0x1B,  // Bit 1 strobe
        0x19, 0x1B,  // Bit 0 strobe
    );
}

void test_syncTransact(void) {
    spi_initMaster(SPI_USE_MOSI);
    reset_history();
    sequence(USIDR, 0x00);  // write
    sequence(USIDR, 0xAB);  // read back

    uint8_t val = spi_syncTransact(0xBC);
    assert_int_equal(0xAB, val);
    assert_history(USIDR, 0xBC, 0xAB);
    assert_history(USISR, 0x40);
    assert_history(USICR,
        0x1A, 0x1A,  // Reading twice to set regs
        0x19, 0x1B,  // Bit 7 strobe
        0x19, 0x1B,  // Bit 6 strobe
        0x19, 0x1B,  // Bit 5 strobe
        0x19, 0x1B,  // Bit 4 strobe
        0x19, 0x1B,  // Bit 3 strobe
        0x19, 0x1B,  // Bit 2 strobe
        0x19, 0x1B,  // Bit 1 strobe
        0x19, 0x1B,  // Bit 0 strobe
    );
}

int main(void) {
    test(test_init_mosi);
    test(test_init_miso);
    test(test_init_invert);
    test(test_syncWrite);
    test(test_syncTransact);
    test(test_syncRead);
    return 0;
}
