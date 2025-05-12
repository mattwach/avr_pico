#ifndef AVR_INTERRUPT_H
#define AVR_INTERRUPT_H

#include "io.h"

static inline void cli(void) {
    SREG &= 0x7F;
}

static inline void sei(void) {
    SREG |= 0x80;
}

#endif