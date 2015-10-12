# WARNING: This project has not yet successfully run on actual hardware!

# is31io7326_clone
Functional clone of the IS31IO7326 using an ATtiny48

## Getting started:

1. Install an avr toolchain (if you're on OS X, try [CrossPack for AVR®](https://www.obdev.at/products/crosspack/))
2. `cd firmware`
3. `make`

`make flash` is configured to use [USBtinyISP](https://learn.adafruit.com/usbtinyisp). To use a different programmer, change `PROGRAMMER` in `firmware/Makefile`

## Porting to different hardware

It's possible to configure this project for different AVRs. Check out [`config/attiny48.h`](https://github.com/numist/is31io7326_clone/blob/master/firmware/config/attiny48.h) to get familiar with the required definitions and application considerations.
