#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "debounce.h"
#include "ringbuf.h"
#include "twi-slave.h"

#if defined (__AVR_ATtiny48__)
# include "config/attiny48.h"
#else
# error No port configuration found for hardware
#endif

#define TWI_BASE_ADDRESS     (0x58 >> 1)

/* NOTE:
 * The indirection operator is to make it obvious that the function-like macro is destructive.
 * gcc is smart enough to optimize it out in combination with the address-of operator at the call site.
 */
#define BIT_RESET(val, off) (*(val) &= ~(1 << (off)))
#define BIT_IS_SET(val, off) (((val) & (1 << (off))) != 0)

typedef union {
    struct {
        uint8_t dataNumber:1,
                keyState:1,
                od:3,
                pp:3;
    };
    uint8_t val;
} key_t;

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

bool twi_handler_unsafe = false;
bool twi_event_pending = false;
unsigned char messageBuf[TWI_BUFFER_SIZE];

void twi_handle_event(void)
{
    twi_event_pending = true;

    // This is called from an interrupt, the ringbuf may be mid-insertion.
    if (twi_handler_unsafe) {
        return;
    }

    // Check if the TWI Transceiver has completed an operation.
    if (!TWI_Transceiver_Busy()) {
        // Check if the last operation was successful
        if (TWI_statusReg.lastTransOK) {
            // Check if the last operation was a reception
            if (TWI_statusReg.RxDataInBuf) {
                TWI_Get_Data_From_Transceiver(messageBuf, 1);         

                // 0x10 Read Key Status Register
                if (messageBuf[0] == 0x10)
                {
                    messageBuf[0] = 1;
                    TWI_Start_Transceiver_With_Data( messageBuf, 1);
                    // TODO: INT pin
                }
            } else {
                // Ends up here if the last operation was a transmission
            }
        }
    }

    if (!TWI_Transceiver_Busy()) {
      TWI_Start_Transceiver();
    }

    twi_event_pending = false;
}

static inline void setup(void)
{
    DDR_PP = 0xFF;
    PIN_PP = 0xFF;
    
    DDR_OD = 0x00;
    PIN_OD = 0xFF;

    // TODO: AD01
    TWI_Slave_Initialise(TWI_BASE_ADDRESS << TWI_ADR_BITS);
    TWI_Start_Transceiver();
    TWI_event = twi_handle_event;
    sei();
}

static inline void loop(void)
{
    for (uint8_t pp = 0; pp < 8; ++pp) {
        PIN_PP = 0xFF ^ (1 << pp);
        
        _delay_us(0.5);
        
        uint8_t changes = debounce(PIN_OD, db + pp);

        if (changes) {
            twi_handler_unsafe = true;
            for (int8_t od = __builtin_ffs(changes) - 1; od >= 0; BIT_RESET(&changes, od)) {
                ringbuf_append(((key_t){
                    .dataNumber = 0, // Set by I²C slave when ringbuf.count > 0
                    .keyState = !BIT_IS_SET(db[pp].state, od),
                    .od = od,
                    .pp = pp
                }).val);
            }
            // TODO: INT pin
            twi_handler_unsafe = false;
        }
        if (twi_event_pending) {
            twi_handle_event();
        }
        
        _delay_us(0.5);
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
