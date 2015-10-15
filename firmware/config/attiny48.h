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

// Scanning port (N/C when all keys are up)
#define PORT_PP PORTB
#define DDR_PP DDRB
#define PIN_PP PINB

// Signal port (expect pins to be pulled up to Vᴄᴄ by 4.7KΩ)
#define PORT_OD PORTD
#define DDR_OD DDRD
#define PIN_OD PIND

#define INT_PIN 7
#define INT_MASK (1 << INT_PIN)
#define INIT_INT() (DDRC = DDRC | INT_MASK)
#define SET_INT(val) (PORTC = ((val) ? (PINC | INT_MASK) : (PINC & ~INT_MASK) ))
