#include "is31io7326.h"
#include "main.h"
#include "ringbuf.h"
#include "twi-slave.h"

uint8_t issi_config = 0x10;

void issi_init(void)
{
    SET_INT(1);

    TWI_Rx_Data_Callback = issi_twi_data_received;
    TWI_Tx_Data_Callback = issi_twi_data_requested;

    // TODO: set TWI_Tx_Data_Callback and TWI_Rx_Data_Callback
    TWI_Slave_Initialise(TWI_BASE_ADDRESS | AD01());
    sei();
}

static uint8_t issi_twi_command = 0;

void issi_twi_data_received(uint8_t *buf, uint8_t bufsiz) {
    if (__builtin_expect(bufsiz != 0, 1)) {
        if (buf[0] == 0x08 && bufsiz > 1) {
            // SET configuration
            issi_config = buf[1];
            SET_INT(1);
        } else {
            // GET configuration
            issi_twi_command = buf[0];
        }
    }
}

void issi_twi_data_requested(uint8_t *buf, uint8_t *bufsiz) {
    if (__builtin_expect(*bufsiz != 0, 1)) {
        if (issi_twi_command == 0x08) {
            // Configuration Register
            buf[0] = issi_config;
            *bufsiz = 1;
        } else if (issi_twi_command == 0x10) {
            // Key Status Register
            key_t key;
            if (ringbuf_empty()) {
                // TODO: keyState needs to be set based on whether key 0 is already down
                key.val = 0;
            } else {
                key.val = ringbuf_pop();
                if (ringbuf_empty()) {
                    SET_INT(1);
                } else {
                    key.dataNumber = 1;
                }
            }
            buf[0] = key.val;
            *bufsiz = 1;
        }
    }
}
