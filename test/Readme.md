AVR Library Unit test framework.

Like the entire project, the idea is to get the job done with as little
complexity as possible.

# Basic Setup

Here is a working example in the context of `lib/uart/uart.c`.

  1. First we create a `lib/uart/uart_test.c` object file.
  2. This file should include `test/unit_test.h` first and the header under test (`lib/uart.h`) second.
  3. Call tets functions whatever you want, but starting them with `test_` is probably helpful for a future reader.
  4. The tests should be small and target a feature of a given `uart.h` function
  5. Call the functions in `uart.h` directly.  The unit test framework should provide the right headers.
  6. Call `assert_*` functions to check that the hardware register values are correct.

# AVR Hardware

Each AVR hardware register is presented as a macro that looks like this.

    #define REG *(REG_history[REG_index++])
    uint8_t REG_history[1024];
    int REG_index;

This if the code calls:

    REG = 5;
    REG = 8;

It compiles as:

    *(REG_history[REG_index++]) = 5;
    *(REG_history[REG_index++]) = 8;

and you can then access `REG_history` and `REG_index` in your asserts.

    assert_equal(REG_index, 2);
    assert_bytes_equal({5, 8}, REG_history, 1);

# AVR Interrupts

Sometimes setting a register will arm an interrupt.  For example:

    ISR(USART_TX_vect) {
        if (index < length) {
            UDR0 = buffer[index++];
        }
    }

Not only sets `UDR0` but also triggers a future call to `USART_TX_vect` after
the hardware writes the byte.

How can we unit test this?

Well, registers like this are defined differently, like this:

    #define UDR0 "Do not set UDR0 directly, please see test/Readme.md"

Whereas `uart.c` needs a definition like this:

    #ifndef SET_UDR0
    #define SET_UDR0(v) UDR0 = (v)
    #endif

and change the access to:

    ISR(USART_TX_vect) {
        if (index < length) {
            SET_UDR0(buffer[index++]);
        }
    }

Now the real code is basially idential but the test code will call the
special handler function.

For the tested code (`uart.c` above), we need to indicate that the callback
version of the set should be used when compiled as a test case module (since there
is no actual hardware in that context).

  #define USE_USART_TX_vect
  // before this include
  #include <io.h>

If we do not do this, the value will be set without calling the associated
interrupt handler. This is done this way to avoid having to define a function
for every possible interrupt handler, you instead just override the set for the ones
that exist.
