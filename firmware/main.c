#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "debounce.h"
#include "is31io7326.h"
#include "main.h"
#include "ringbuf.h"
#include "twi-slave.h"

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

void twi_data_received( uint8_t *buf, uint8_t bufsiz);
void twi_data_requested( uint8_t *buf, uint8_t *bufsiz);

static inline void setup(void)
{
    DDR_PP = 0x00;
    PORT_PP = 0xFF;

    DDR_OD = 0x00;
    PORT_OD = 0xFF;

    SET_INT(1);

    TWI_Rx_Data_Callback = twi_data_received;
    TWI_Tx_Data_Callback = twi_data_requested;

    // TODO: set TWI_Tx_Data_Callback and TWI_Rx_Data_Callback
    TWI_Slave_Initialise(TWI_BASE_ADDRESS | AD01());
    sei();
}

static inline void loop(void)
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
        bool nPp = __builtin_popcount(~PIN_PP);
        bool nOd = __builtin_popcount(~od_bits);
        // Most of the time the keyboard will not be a rollover state
        if (__builtin_expect(nPp != 1 && nOd != 1, 0)) {
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

int main(void)
{
    setup();
    while(1){
        loop();
    }
    __builtin_unreachable();
}

uint8_t twi_command = 0;
uint8_t issi_config = 0x10;

void twi_data_received(uint8_t *buf, uint8_t bufsiz) {
    if (__builtin_expect(bufsiz != 0, 1)) {
        if (buf[0] == 0x08 && bufsiz > 1) {
            // SET configuration
            issi_config = buf[1];
        } else {
            // GET configuration
            twi_command = buf[0];
        }
    }
}

void twi_data_requested(uint8_t *buf, uint8_t *bufsiz) {
    if (__builtin_expect(*bufsiz != 0, 1)) {
        if (twi_command == 0x08) {
            // Configuration Register
            buf[0] = issi_config;
            *bufsiz = 1;
        } else if (twi_command == 0x10) {
            // Key Status Register
            if (ringbuf_empty()) {
                *bufsiz = 0;
            } else {
                buf[0] = ringbuf_pop();
                *bufsiz = 1;
                if (ringbuf_empty()) {
                    SET_INT(1);
                }
            }
        }
    }
}
