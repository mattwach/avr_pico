#include <test/unit_test.h>
#include "twi.h"

void test_init(void) {
  twi_init();
  twi_init(); // This should do nothing
  assert_reg_equal(0x05, DDRB);  // Ports 0 and 2 set as output
  assert_reg_equal(0x05, PORTB); // Ports 0 and 2 enabled

  // 0 !Start Condition Interupt enable
  // 0 !Counter Overflow Interrupt enable
  // 1 Two Wire Mode
  // 0 ^
  // 1 External, positive edge clock, software strobe (USITC)
  // 0 ^
  // 1 ^
  // 0 !Clock Toggle
  assert_reg_equal(0x2A, USICR);

  // The code indicates that 0xFF is a good reseting value for
  // USIDR (e.g. a resting level).  I have not dug any further
  // into it.
  assert_reg_equal(0xFF, USIDR);

  // 1 Start Condition Interrupt flag
  // 1 Counter Overflow Interrupt flag
  // 1 Stop Condition flag
  // 1 Data output collision
  // 0 Counter value
  // 0 ^
  // 0 ^
  // 0 ^
  assert_reg_equal(0xF0, USISR);
}

static void common_init(void) {
  twi_reinit();
  reset_history();
}

void test_startWrite_OK(void) {
  error_t error = 0;
  common_init();
  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USIDR, DONT_CARE); // Data write
  sequence(USIDR, 0x00); // disable the NACK on 2nd USI_ShiftData
  sequence(USIDR, DONT_CARE); // Seting for write
  sequence(USIDR, 0x00); // disable the NACK on 2nd USI_ShiftData

  twi_startWrite(0x7b, &error);

  assert_history(PORTB,
    // StartBit
    // expected initial state of 0x05
    0x05, // SCL high (but was already high)
    0x04, // SDA Low
    0x00, // SCL Low
    0x01, // SDA high again

    // Write Byte
    0x01, // SCL Low (already low)
  );
  assert_history(USISR,
    // Start bit
    0xF0, // init conditon

    // USI_ShiftData (8-bit)
    0xF0, // USISR_8BIT
    0xF0, // while loop

    // USI_ShiftData (1-bit)
    0xFE,
    0xFE
  );
  assert_history(USIDR,
    0xF6,  // 0x7B << 1 (address write)
    0x00,  // ACK
    0xFF,  // Release
    0x00,  // Ack
    0xFF,  // Release
  );
  assert_history(DDRB,
    // initial state should be 0x05
    0x05,  // SDA as input (already was)
    0x04,  // SDA as input
    0x05,  // SDA as output again
  );
  assert_history(USICR,
    // Initial state 0x2A
    0x2B,  // All of these are using in ShiftData to gen a positive edge
    0x2B,
    0x2B,
    0x2B
  );
  assert_int_equal(0, error);
}

void test_startWrite_preerror(void) {
  common_init();
  error_t error = 1;

  twi_startWrite(0x7b, &error);

  assert_int_equal(1, error);
}

void test_startWrite_MissingStartCon(void) {
  common_init();
  error_t error = 0;

  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USISR, 0);  // Turn off start con
  twi_startWrite(0x7b, &error);

  assert_int_equal(TWI_MISSING_START_CON_ERROR, error);

  assert_reg_activity(PORTB); // Already covered in the OK test
}

void test_startWrite_NoAckOnAddress(void) {
  error_t error = 0;
  common_init();
  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USIDR, DONT_CARE); // Data write
  sequence(USIDR, DONT_CARE); // 8-bit check
  sequence(USIDR, DONT_CARE);  // 1 bit write
  sequence(USIDR, 0x01);  // NACK

  twi_startWrite(0x7b, &error);

  assert_reg_activity(PORTB); // Already covered in the OK test
  assert_reg_activity(USISR); // Already covered in the OK test
  assert_reg_activity(DDRB); // Already covered in the OK test
  assert_reg_activity(USICR); // Already covered in the OK test

  assert_history(USIDR,
    0xF6,  // 0x7B << 1 (address write)
    0xDC,  // 8-bit check
    0xFF,  // USI_ShiftData likes to set this to 0xFF
    0x01,  // Nack
    0xFF,  // USI_ShiftDAta likes to set this to 0xFF
  );

  assert_int_equal(TWI_NO_ACK_ERROR, error);
}

void test_writeNoStop_OK(void) {
  error_t error = 0;
  common_init();

  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USIDR, DONT_CARE); // Data write
  sequence(USIDR, 0x00); // disable the NACK on 2nd USI_ShiftData
  sequence(USIDR, DONT_CARE); // Seting for write
  sequence(USIDR, 0x00); // disable the NACK on 2nd USI_ShiftData

  twi_writeNoStop(0xAB, &error);

  assert_int_equal(0, error);
  assert_history(USICR,
    // Initial state 0x2A
    0x2B,  // All of these are using in ShiftData to gen a positive edge
    0x2B,
    0x2B,
    0x2B
  );
  assert_history(PORTB, 0x01);  // SCL pulled low
  assert_history(DDRB,
    0x05,  // Enable SDA (already enabled)
    0x04,  // enable SDA as inut
    0x05,  // Enavlew as ouput again
  );
  assert_history(USISR,
    0xF0, // USISR_8BIT
    0xF0, // while loop

    // USI_ShiftData (1-bit)
    0xFE,
    0xFE
  );
}

void test_writeNoStop_preerror(void) {
  common_init();
  error_t error = 1;

  twi_writeNoStop(0xab, &error);

  assert_int_equal(1, error);
}

void test_writeNoStop_NACK(void) {
  error_t error = 0;
  common_init();

  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USIDR, DONT_CARE); // Data write
  sequence(USIDR, 0x00); // disable the NACK on 2nd USI_ShiftData
  sequence(USIDR, DONT_CARE); // Seting for write
  sequence(USIDR, 0x01); // NACK

  twi_writeNoStop(0xAB, &error);

  assert_int_equal(TWI_NO_ACK_ERROR, error);
  // Presume the OK is covering these
  assert_reg_activity(USICR);
  assert_reg_activity(PORTB);
  assert_reg_activity(DDRB);
  assert_reg_activity(USISR);
}

void test_readNoStop_OK(void) {
  error_t error = 0;
  uint8_t data[3];
  common_init();
  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang

  sequence(USIDR, DONT_CARE); // Address
  sequence(USIDR, DONT_CARE); // Read data
  sequence(USIDR, DONT_CARE); // Set to 0xFF
  sequence(USIDR, 0x00); // ACK
  sequence(USIDR, DONT_CARE); // Set to 0xFF

  sequence(USIDR, 'H'); // Read within shift data
  sequence(USIDR, DONT_CARE); // 0xFF clear
  sequence(USIDR, DONT_CARE); // 0x00 Ack
  sequence(USIDR, DONT_CARE); // 1 bit read
  sequence(USIDR, DONT_CARE); // 0xFF clear

  sequence(USIDR, 'e'); // Read within shift data
  sequence(USIDR, DONT_CARE); // 0xFF clear
  sequence(USIDR, DONT_CARE); // 0x00 Ack
  sequence(USIDR, DONT_CARE); // 1 bit read
  sequence(USIDR, DONT_CARE); // 0xFF clear

  sequence(USIDR, 'y'); // Read within shift data
  sequence(USIDR, DONT_CARE); // 0xFF clear
  sequence(USIDR, DONT_CARE); // 0xFF NACK
  sequence(USIDR, DONT_CARE); // 1 bit read
  sequence(USIDR, DONT_CARE); // 0xFF clear

  twi_readNoStop(0x7b, data, 3, &error);

  assert_int_equal(0, error);
  assert_buff_equal((uint8_t*)"Hey", data, 3);

  assert_history(USIDR,
    // WriteByte for address
    0xF7,  // 0x7B << 1 | 1 (address read)
    0xDC,  // Read data
    0xFF,  // Set to 0xFF
    0x00,  // ACK
    0xFF,  // Set to 0xFF

    0x48,  // 'H'
    0xFF,  // Set to 0xFF
    0x00,  // readCommon ack
    0xDC,  // 1-bit read
    0xFF,  // Set to 0xFF

    0x65,  // 'e'
    0xFF,  // Set to 0xFF
    0x00,  // readCommon ack
    0xDC,  // 1-bit read
    0xFF,  // Set to 0xFF

    0x79,  // 'y'
    0xFF,  // Set to 0xFF
    0xFF,  // readCommon NACK
    0xDC,  // 1-bit read
    0xFF,  // Set to 0xFF
  );

  assert_history(PORTB,
    // StartBit
    // expected initial state of 0x05
    0x05, // SCL high (but was already high)
    0x04, // SDA Low
    0x00, // SCL Low
    0x01, // SDA high again

    // Write Byte
    0x01, // SCL Low (already low)
  );

  assert_history(DDRB,
    // WriteByte
    0x05,  // ShiftData, Enable SDA
    0x04,  // WriteByte Disable SDA
    0x05,  // ShiftData Enable SDA

    // Read loop 'H'
    0x04, // Disable SDA
    0x05,  // ShiftData, Enable SDA
    0x05,  // ShiftData, Enable SDA

    // Read loop 'e'
    0x04, // Disable SDA
    0x05,  // ShiftData, Enable SDA
    0x05,  // ShiftData, Enable SDA

    // Read loop 'y'
    0x04, // Disable SDA
    0x05,  // ShiftData, Enable SDA
    0x05,  // ShiftData, Enable SDA
  );

  assert_history(USICR,
    // All from ShiftData
    0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
  );

  assert_history(USISR,
    // USI_Start
    0xF0,  // Check for complete

    // USI_WriteByte (Address)
    0xF0, // USISR_8BIT
    0xF0, // loop
    0xFE, // USISR_1BIT
    0xFE, // loop

    // read 'H'
    0xF0, // USISR_8BIT
    0xF0, // loop
    0xFE, // USISR_1BIT
    0xFE, // loop

    // read 'e'
    0xF0, // USISR_8BIT
    0xF0, // loop
    0xFE, // USISR_1BIT
    0xFE, // loop

    // read 'y'
    0xF0, // USISR_8BIT
    0xF0, // loop
    0xFE, // USISR_1BIT
    0xFE, // loop
  );
}

void test_readNoStop_preerror(void) {
  common_init();
  error_t error = 1;

  twi_readNoStop(0x7a, 0, 0, &error);

  assert_int_equal(1, error);
}

void test_readNoStop_MissingStartCon(void) {
  common_init();
  error_t error = 0;

  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USISR, 0);  // Turn off start con
  twi_readNoStop(0x7b, 0, 0, &error);

  assert_int_equal(TWI_MISSING_START_CON_ERROR, error);

  assert_reg_activity(PORTB); // Already covered in the OK test
}

void test_readNoStop_NoAckOnAddress(void) {
  error_t error = 0;
  common_init();
  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang
  sequence(USIDR, DONT_CARE); // Data write
  sequence(USIDR, DONT_CARE); // 8-bit check
  sequence(USIDR, DONT_CARE);  // 1 bit write
  sequence(USIDR, 0x01);  // NACK

  twi_readNoStop(0x7b, 0, 0, &error);

  assert_reg_activity(PORTB); // Already covered in the OK test
  assert_reg_activity(USISR); // Already covered in the OK test
  assert_reg_activity(DDRB); // Already covered in the OK test
  assert_reg_activity(USICR); // Already covered in the OK test

  assert_history(USIDR,
    0xF7,  // 0x7B << 1 | 1 (address read)
    0xDC,  // 8-bit check
    0xFF,  // USI_ShiftData likes to set this to 0xFF
    0x01,  // Nack
    0xFF,  // USI_ShiftDAta likes to set this to 0xFF
  );

  assert_int_equal(TWI_NO_ACK_ERROR, error);
}

void test_stop(void) {
  error_t err = 0;
  common_init();
  sequence(PINB, 0x04); // Show SCL as high to avoid poll hang

  twi_stop(&err);

  assert_int_equal(0, err);
  assert_history(PORTB,
      0x04, // SDA Low
      0x04, // SCL High
      0x05  // SDA High
  );

  assert_history(
      USISR,
      0xF0, // State read
  );
}

void test_stop_preerror(void) {
  error_t err = 1;
  common_init();

  twi_stop(&err);

  assert_int_equal(1, err);
}

int main(void) {
  test(test_init);
  
  test(test_startWrite_OK);
  test(test_startWrite_preerror);
  test(test_startWrite_MissingStartCon);
  test(test_startWrite_NoAckOnAddress);

  test(test_writeNoStop_OK);
  test(test_writeNoStop_preerror);
  test(test_writeNoStop_NACK);

  test(test_readNoStop_OK);
  test(test_readNoStop_preerror);
  test(test_readNoStop_MissingStartCon);
  test(test_readNoStop_NoAckOnAddress);

  test(test_stop);
  test(test_stop_preerror);

  return 0;
}
