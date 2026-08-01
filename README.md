# Simple Scaled Image Demo
This is a graphical demo which runs on:

* Raspberry Pi Pico 2W, with
* Waveshare 1.69" LCD display.
* A pull-up resistor button

# How It Looks

The demo shows this animation:

<img src="https://raw.githubusercontent.com/clandrew/rpimageload/main/Demo/Sprite.gif" />

Photo of the screen:

<img src="https://raw.githubusercontent.com/clandrew/rpimageload/main/Demo/Screen.png" />

Video, when the button pressed to toggle the color invert effect:

<img src="https://raw.githubusercontent.com/clandrew/rpimageload/main/Demo/Video.gif" />

# Setup
Use the following connections:

WaveShare LCD pin | Raspberry Pi Pico 2W semantic pin | Raspberry Pi Pico 2W global pin
--- | --- | --- 
VCC | 3v3 | Pin 39  
GND | GND | Pin 38
DIN | GP19, SPI0 TX | Pin 25
CLK | GP18, SPI0 SCK | Pin 24
CS | GP17, SPI0 CSn |  Pin 22
DC | GP22 |  Pin 29  
RST | GP13 (also SPI1CSn)  |  Pin 17
BL | GP21 |  Pin 27

Button pin | Raspberry Pi Pico 2W semantic pin | Raspberry Pi Pico 2W global pin
--- | --- | --- 
Button (red) | GP14 | Pin 19
Button (black) | GND | Pin 18

The display uses SPI interface. You can set it up differently from how it's listed here, but you need to check that the constants in DEV_Config.h refer to the right GPIO.

# Controls
Press the button to invert the colors of the image. Pressing the button again will return them to normal.

# How it works
The demo loads pre-formatted image data, hardcoded in a file.

As for the pre-formatted image data, I made a tool called ColorConversion that takes an input image file (e.g., a PNG file) and converts it to the R5G6B5-with-flipped-endianness that the platform expects. The included demo images were set up using the tool with these command lines

```
ColorConversion.exe test_image_1.png test_image_1.data.h
ColorConversion.exe test_image_2.png test_image_2.data.h
ColorConversion.exe test_image_3.png test_image_3.data.h
```
The images are all a quarter screen size. The demo does a 4x scaled blit to make them fullscreen.

# Build
The actual democode is all in one file, rpimageload.ino.

The code is in C++, with some boilerplate pulled in based off of Raspberry Pi and WaveShare samples.

The sketch is compiled and uploaded using Arduino IDE. Tested with Arduino IDE 2.3.10.

