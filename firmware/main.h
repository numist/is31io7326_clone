#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined (__AVR_ATtiny48__)
# include "config/attiny48.h"
#else
# error No port configuration found for hardware
#endif
#include "config/validate.h"

#define SET_OUTPUT(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define SET_INPUT(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

#define HIGH(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define LOW(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

#define SET_INT(val) do { \
    if (val) { \
        SET_INPUT(DDR_INT, PIN_NO_INT); \
        HIGH(PORT_INT, PIN_NO_INT); \
    } else { \
        LOW(PORT_INT, PIN_NO_INT); \
        SET_OUTPUT(DDR_INT, PIN_NO_INT); \
    } \
} while(0)

#define DISABLE_INTERRUPTS(code) do{ \
    cli(); \
    { \
        code \
    } \
    sei(); \
}while(0)
