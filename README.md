# WARNING: This project has not yet successfully run on actual hardware!

# is31io7326_clone
Functional clone of the IS31IO7326 using an ATtiny48

## Getting started:

1. Install an avr toolchain (if you're on OS X, try [CrossPack for AVR®](https://www.obdev.at/products/crosspack/))
2. `cd firmware`
3. `make`

`make flash` is configured to use [USBtinyISP](https://learn.adafruit.com/usbtinyisp). To use a different programmer, change `PROGRAMMER` in `firmware/Makefile`
