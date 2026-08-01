# Simple Scaled Image Demo
This is a graphical demo which runs on:

* Raspberry Pi Pico 2W, with
* Waveshare 1.69" LCD display.
* A pull-up resistor button

The demo loads pre-formatted image data, hardcoded in a file. The image is set up to be a quarter screen size. Then it does a 4x scaled blit to video memory.

As for the pre-formatted image data, there's a tool called ColorConversion that takes an input image file (e.g., a PNG file) and converts it to the R5G6B5-with-flipped-endianness that the platform expects.
