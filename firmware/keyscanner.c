#include <util/delay.h>
#include "debounce.h"
#include "is31io7326.h"
#include "main.h"
#include "ringbuf.h"

debounce_t db[] = {
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF}
};

void keyscanner_init(void)
{
    DDR_PP = 0x00;
    PORT_PP = 0xFF;

    DDR_OD = 0x00;
    PORT_OD = 0xFF;
}

static inline uint8_t popCount(uint8_t val) {
    uint8_t count;
    for (count=0; val; count++) {
        val &= val-1;
    }
    return count;
}

void keyscanner_main(void)
{
    /* TODO: low power mode:
     *   When all keys reported up:
     *     DDR_PP = 0x11; PORT_PP = 0x00;
     *     Guarantee wake on TWI / any PORT_OD pin FALLING
     *     Sleep
     */
    for (uint8_t pp = 0; pp < 8; ++pp) {
        uint8_t pp_bitmask = _BV(pp);

        _delay_ms(0.5);

        DDR_PP = 0x00 ^ pp_bitmask;
        PORT_PP = 0xFF ^ pp_bitmask;

        _delay_ms(0.5);

        uint8_t od_bits = PIN_OD;
        /*
         * Rollover conditions exist if:
         *  * Multiple OD pins are pulled low AND
         *  * Multiple PP pins are pulled low
         */
        uint8_t nPp = popCount(~PIN_PP);
        uint8_t nOd = popCount(~od_bits);
        // Most of the time the keyboard will not be a rollover state
        if (__builtin_expect(nPp > 1 && nOd > 1, 0)) {
            continue;
        }

        // Debounce key state
        uint8_t changes = debounce(od_bits, db + pp);
        // Most of the time there will be no new key events
        if (__builtin_expect(changes == 0, 1)) {
            continue;
        }

        DISABLE_INTERRUPTS({
            key_t key;
            key.dataNumber = 0; // Set by I²C code (ringbuf.count != 0)
            key.pp = pp;

            for (int8_t od = 0; od < 8; od++) {
                // Fewer than half the keys are expected to be down for each scanline
                if (__builtin_expect(bit_is_set(changes, od), 0)) {
                    key.keyState = bit_is_clear(db[pp].state, od);
                    key.od = od;
                    ringbuf_append(key.val);
                }
            }

            SET_INT(0);
        });
    }
}
