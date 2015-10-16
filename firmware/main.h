#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined (__AVR_ATtiny48__)
# include "config/attiny48.h"
#else
# error No port configuration found for hardware
#endif
#include "config/validate.h"

#define INT_MASK (1 << PIN_NO_INT)
#define SET_INT(val) do { \
    if (val) { \
        PORT_INT |= INT_MASK; \
    } else { \
        PORT_INT &= ~INT_MASK; \
    } \
} while(0)

#define DISABLE_INTERRUPTS(code) do{ \
    cli(); \
    { \
        code \
    } \
    sei(); \
}while(0)
