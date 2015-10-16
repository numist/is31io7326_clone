#pragma once

/*
 * Application considerations:
 * The ATtiny48/88 has three 8-bit ports available.
 * Two have special functionality of interest to the application:
 *     PORTB: used by ISP
 *     PORTC: used by TWI/I²C
 *
 * For complete compatibility with IS31IO7326, the OD port should be expected to
 * be populated with external pull-up resistors, making it unsuitable for ISP.
 */

// PP: Scanning port (N/C when all keys are up)
#define PORT_PP PORTB
#define DDR_PP DDRB
#define PIN_PP PINB

// OD: Signal port (expect pins to be pulled up to Vᴄᴄ by 4.7KΩ)
#define PORT_OD PORTD
#define DDR_OD DDRD
#define PIN_OD PIND

// INT: Interrupt pin
#define PIN_NO_INT 7
#define PORT_INT PORTC
#define DDR_INT DDRC
#define PIN_INT PINC

// AD01: lower two bits of device address
#define AD01() (PINC & 0x03)
