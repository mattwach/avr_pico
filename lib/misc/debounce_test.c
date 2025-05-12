#include "debounce.h"

#include <test/unit_test.h>
#include <string.h>

void test_init(void) {
  struct Debounce db;
  struct Debounce expected_db;
  memset(&expected_db, 0, sizeof(struct Debounce));
  expected_db.hold_time_ms = 123;
  debounce_init(&db, 123);
  assert_int_equal(0, memcmp(&expected_db, &db, sizeof(struct Debounce)));
}

void test_debounce(void) {
  struct Debounce db;
  debounce_init(&db, 10);

  assert_int_equal(1, debounce_sample(&db, 1, 111));  // first call
  assert_int_equal(111, db.val);

  assert_int_equal(0, debounce_sample(&db, 1, 112));  // ignore
  assert_int_equal(111, db.val);

  assert_int_equal(0, debounce_sample(&db, 2, 113));  // ignore
  assert_int_equal(111, db.val);

  assert_int_equal(0, debounce_sample(&db, 10, 114));  // ignore
  assert_int_equal(111, db.val);

  assert_int_equal(0, debounce_sample(&db, 10, 115));  // ignore
  assert_int_equal(111, db.val);

  assert_int_equal(1, debounce_sample(&db, 11, 116));  // triggered
  assert_int_equal(116, db.val);

  assert_int_equal(0, debounce_sample(&db, 11, 117));  // ignore
  assert_int_equal(116, db.val);

  assert_int_equal(0, debounce_sample(&db, 20, 118));  // ignore
  assert_int_equal(116, db.val);

  assert_int_equal(1, debounce_sample(&db, 21, 119));  // triggered
  assert_int_equal(119, db.val);

  assert_int_equal(1, debounce_sample(&db, 40, 120));  // triggered
  assert_int_equal(120, db.val);
}

void test_debounce_gpio_irq_callback_helper(void) {
  struct Debounce db;
  debounce_init(&db, 10);
  const uint8_t fall = 0x04;
  const uint8_t rise = 0x08;

  // normal op with a little bounce on both ends
  assert_int_equal(1, debounce_gpio_irq_callback_helper(&db, 1, fall));
  assert_int_equal(1, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 1, rise));
  assert_int_equal(1, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 10, fall));
  assert_int_equal(1, db.val);
  assert_int_equal(1, debounce_gpio_irq_callback_helper(&db, 11, rise));
  assert_int_equal(0, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 11, fall));
  assert_int_equal(0, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 20, rise));
  assert_int_equal(0, db.val);

  // replace the initial conditions with doubles
  assert_int_equal(1, debounce_gpio_irq_callback_helper(&db, 1, rise | fall));
  assert_int_equal(1, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 1, rise));
  assert_int_equal(1, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 10, fall));
  assert_int_equal(1, db.val);
  assert_int_equal(1, debounce_gpio_irq_callback_helper(&db, 11, rise | fall));
  assert_int_equal(0, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 11, fall));
  assert_int_equal(0, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 20, rise));
  assert_int_equal(0, db.val);

  // make it ambiguous everywhere
  assert_int_equal(1, debounce_gpio_irq_callback_helper(&db, 1, rise | fall));
  assert_int_equal(1, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 1, rise | fall));
  assert_int_equal(1, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 10, rise | fall));
  assert_int_equal(1, db.val);
  assert_int_equal(1, debounce_gpio_irq_callback_helper(&db, 11, rise | fall));
  assert_int_equal(0, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 11, rise | fall));
  assert_int_equal(0, db.val);
  assert_int_equal(0, debounce_gpio_irq_callback_helper(&db, 20, rise | fall));
  assert_int_equal(0, db.val);
}

int main(void) {
  test(test_init);
  test(test_debounce);
  test(test_debounce_gpio_irq_callback_helper);
  return 0;
}
