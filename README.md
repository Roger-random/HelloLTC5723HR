# Volt Meter Exercise with PIC16F18345 and LTC-5723HR

This repository holds code for a PIC microcontroller programming exercise,
taking a PIC16F18345 and turning it into a crude volt meter using the
chip's built in FVR (fixed voltage reference) peripheral. The measured
voltage is converted to user-readable digits displayed on a LTC-5723HR
seven-segment LED display. The seven segments (plus decimal point for
eight LEDs total) are connected to the PIC's port C so a digit update can
be sent out in a single byte to LATC.

Demonstrates usage of:
* FVR (fixed voltage reference) peripheral
* ADC (analog to digital converter) peripheral
* ISR (interrupt service routine) to keep LED refreshed

More details at https://newscrewdriver.com/2017/09/24/pic-exercise-volt-meter/

---

## License

_Most of the code are standard boilerplate code provided by Microchip Technology Inc. and
subject to their licensing restrictions. See source code files for details._

_Code created by me, Roger Cheng, is released under the MIT license._