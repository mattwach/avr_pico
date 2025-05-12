// Unit testing framework for AVR
// Sets up some simple fake hardware, resets it before eeach test and provides
// some asserts

#include <avr/io_common.h>
#include <inttypes.h>
#include <string.h>

// Runs a test.
//
// The test should start with the the prefix '_test' and no non-test
// functions should have this prefix (otherwise the check_test_results.py will
// complain about missing tests).
//
// A test function should be of type void and take no arguments.
//
// Example:
//
// void test_simple(void) {
//   assert_int_equal(0xff, 255);
// }
//
// int main(void) {
//   test(test_simple);
//   return 0;
// }
#define test(X) \
  test_(X, # X)

// Prepares a sequence of return results for a register.  This is essentially
// a faking mechanism that allows a function-under-test to get "expected"
// results and avoid (or enter) error handling code as intended by the
// test.
//
// Example:
// void test_twi_startWrite(void)  {
//  twi_init();
//  sequence(TWSR, 0x10);  // Indicates that TWSR restart was successful
//  sequence(TWSR, 0x18); // Indicates that the Write was ACKed
//  twi_startWrite(0x7B);
//  ...
//}
//
// The above example knows that twi_startWrite() will check TWSR twice and
// expect different values for each check.  Without the sequence, there
// would not be a way to change the value of TWSR and avoid falling into
// twi_startWrite() error handling logic. 
//
// Note that WRITING to a register will also use up a dummy sequence
// slot due to the implementation.  So if the test says:
//
// TWDR = 0xA7;
// if (TWDR = 0x12) {
//   ...
// }
//
// then you need
//
// sequence(TWDR, DONT_CARE);  /// anything at all wil work here
// sequence(TWDR, 0x12);
#define DONT_CARE 0xDC
#define sequence(REG, VAL) \
  sequence_(REG ## _sequence, & REG ## _sequence_max, & REG ## _index, VAL)

// Asserts that a register was read or written without delving into the details.
//
// Calling this prevents the end-of-check tester from complaining that an
// accessed hardware register is not included in a unit test.  You might
// want to do this if interactions are complex and handled by a different test,
// avoind a lot of copy-paste of the same logic in multiple places.
//
// Example:
//
// void test_init(void) {
//   uart_init(9600);
//   assert_reg_equal(0, UBRR0H);
//   assert_reg_equal(103, UBRR0L);
//
//   assert_reg_equal(0x20, UCSR0A);
//   assert_history(SREG, 0x00, 0x80);
// }
// 
// void test_init_57600(void) {
//   uart_init(57600);
//   assert_reg_equal(0, UBRR0H);
//   assert_reg_equal(16, UBRR0L);
//
//   assert_reg_activity(SREG);
//   assert_reg_activity(UCSR0A);
// }
//
// In the above case, we used assert_reg_activity for the 57600 baud case
// to skip registers that would be tested identically in the 9600 baud case.
// This helps lessen the needed test changes if uart_init() itself is changed
// and helps keep focus on what the unit test is actually for (it's not
// for testing things that are alraeady tested, but for the specific changes
// that the 57600 bring).
//
// The tradeoff is that if the 57600 case does change these values in a bad way,
// there is not tracking for it.  Not a perfect solution either way so your
// call.
#define assert_reg_activity(reg_name) \
  assert_reg_activity_(&reg_name ## _index, # reg_name)

// Reset all history to zero, but keep current values
// Useful for allowing an "init" function to set up registers without
// having to add extra checks for the state changed by that init.
//
// For example:
//
// static void test_putbyte() {
//  uart_init(9600);
//  reset_history();
//  uart_putbyte('X');
//  assert_history(UCSR0A, 0x20);
//  assert_history(UDR0, 'X');
//  assert_history(SREG, 0x00, 0x80);
// }
//
// The reset_history() after the uart_init() means that we pick up the values
// changed, which might be relevant, but don't have to specifically test
// those changes here.  This assumes that the init state is being tested
// in a dedicatged unit test.  The idea is to keep the test case focused
// on what uart_putbyte() actually did independently of factor uart_init()
void reset_history(void);

// Asserts that a register was accessed at least once and checks it's latest
// value.
//
// Often, you'll want to use assert_history to check the entire history of
// values, but if that history becomes unweildy, assert_reg_equal can be
// better than assert_reg_activity().  It all depends on if the register
// is already sufficiently covered in other unit tests.
//
// Example:
//
// assert_reg_equal(0x06, UCSR0C);
#define assert_reg_equal(expected, reg_name) \
  assert_reg_equal_( \
    expected, \
    reg_name ## _value, \
    &reg_name ## _index, \
    # reg_name)

// Checks every access to a register - read or write.
//
// Example:
//
// Say your code does this:
//
// PORTB = 0x01
// PORTB |= 0x02
//
// The unit test would say:
//
// assert_history(PORTB, 0x01, 0x12);
//
// The first 0x01 is the assignment
// The 0x12 is the read/write needed for |=
//
// Although the above case would probably be a good candidate for
// assert_reg_equal(PORTB, 0x12);
//
// Since the sequencing here is likely arbitrary (this is not always the
// case, of course).
#define assert_history(reg_name, ...) \
  assert_history_( \
    (const uint8_t[]){__VA_ARGS__}, \
    reg_name ## _history, \
    reg_name ## _value, \
    sizeof((uint8_t[]){__VA_ARGS__}), \
    &reg_name ## _index, \
    # reg_name)

// Checks register access in string form. 
//
// This is mainly useful for convienent testing of I/O registers.
// Note that the '\0' at the end of the string is not used in the test.
//
// Example:
//
// void test_putstr8(void) {
//   ...
//   uart_putstr8("Hello");
//   assert_history_str(UDR0, "Hello");
//   ...
// }
#define assert_history_str(reg_name, str) \
  assert_history_( \
    (const uint8_t*)str, \
    reg_name ## _history, \
    reg_name ## _value, \
    strlen(str), \
    &reg_name ## _index, \
    # reg_name)

// Checks register access of a uint8_t* buffer.
//
// Example:
// 
// void test_putbytes(void) {
//   ...
//   const char* data = "Hello";
//   uart_putbytes8((uint8_t*)data, 5);
//   assert_history_buff(UDR0, data, 5);
//   ...
// }
#define assert_history_buff(reg_name, buff, length) \
  assert_history_( \
    (const uint8_t*)buff, \
    reg_name ## _history, \
    reg_name ## _value, \
    length, \
    &reg_name ## _index, \
    # reg_name)

// Asserts that two buffers are equal.
//
// For registers, prefer assert_history or one of it's variants
//
// Example:
//
// assert_buff_equal((uint8_t*)"Hello", buffer, 5);
#define assert_buff_equal(expected, actual, length) \
  assert_buff_equal_(expected, actual, length, # expected, # actual)
#define assert_str_equal(expected, actual) \
  assert_buff_equal_((uint8_t*)(expected), (uint8_t*)(actual), strlen(expected) + 1, # expected, # actual)

// Asserts that two uint32_t arrays are equal
#define assert_u32_array_equal(expected, actual, length) \
  assert_u32_array_equal_(expected, actual, length, # expected, # actual)

// Asserts that two integers are equal.
//
// For registers, prefer assert_reg_equal.  assert_int_equal does not
// doing the needed bookkeeping to mark the register as tested, which can
// lead to coverage errors at the test's conclusion.
//
// Example:
//
// assert_int_equal(0, error);
#define assert_int_equal(expected, actual) \
  assert_int_equal_(expected, actual, # expected, # actual)

// Asserts that two pointers are equal
#define assert_ptr_equal(expected, actual) \
  assert_ptr_equal_(expected, actual, # expected, # actual)

// Asserts that the second argument is greater than the first
//
// Example:
//
// assert_int_gt(0, bytes_read);
#define assert_int_gt(expected, actual) \
  assert_int_gt_(expected, actual, # expected, # actual)

// Asserts that the second argument is less than the first
//
// Example:
//
// assert_int_lt(100, x);
#define assert_int_lt(expected, actual) \
  assert_int_lt_(expected, actual, # expected, # actual)


// Asserts that two floats are nearly equal.
#define assert_float_near_equal(expected, actual) \
  assert_float_near_equal_(expected, actual, # expected, # actual)

// Asserts that two doubles are nearly equal.
#define assert_double_near_equal(expected, actual) \
  assert_double_near_equal_(expected, actual, # expected, # actual)

// The function below can be called directly if you must, but note that doing so
// can be eror-prone, due to the need to manually construct register names and
// keep things consistent.
void assert_history_(
    const uint8_t* expected_history,
    const uint8_t* actual_history,
    uint8_t current_value,
    uint8_t expected_index,
    const uint16_t* index_reg,
    const char* reg_name);
void assert_int_lt_(
    int expected,
    int actual,
    const char* expected_name,
    const char* actual_name);
void assert_int_gt_(
    int expected,
    int actual,
    const char* expected_name,
    const char* actual_name);
void assert_buff_equal_(
    const uint8_t* expected,
    const uint8_t* actual,
    unsigned int length,
    const char* expected_name,
    const char* actual_name);
void assert_u32_array_equal_(
    const uint32_t* expected,
    const uint32_t* actual,
    unsigned int length,
    const char* expected_name,
    const char* actual_name);
void assert_int_equal_(
    int expected,
    int actual,
    const char* expected_name,
    const char* actual_name);
void assert_ptr_equal_(
    void* expected,
    void* actual,
    const char* expected_name,
    const char* actual_name);
void assert_float_near_equal_(
    float expected,
    float actual,
    const char* expected_name,
    const char* actual_name);
void assert_double_near_equal_(
    double expected,
    double actual,
    const char* expected_name,
    const char* actual_name);
void assert_reg_activity_(
    const uint16_t* index_reg,
    const char* reg_name);
void assert_reg_equal_(
    int expected,
    int actual,
    const uint16_t* index_reg,
    const char* reg_name);
void sequence_(
    uint8_t* sequence_reg,
    uint16_t* sequence_max,
    uint16_t* reg_index,
    uint8_t value);
void test_(void (*fn)(void), const char* test_name);

// These support checking hardware state for untested changes.
extern unsigned int KNOWN_REGISTER_COUNT;
extern uint16_t* const KNOWN_REGISTER_INDEX[];
extern const char* KNOWN_REGISTER_NAME[];
