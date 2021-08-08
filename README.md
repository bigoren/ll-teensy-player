# ll-teensy-player
Teensy library that play led frames from SD card.
Library was written and tested for teensy 3.5, with OctoWS2811 and builtin SD slot on the board.

# Installation
Download the library as zip. open Arduino IDE and go to Sketch -> Include Library -> Add .ZIP library...

# Usage
User should have micro SD card with compatible files, in the format specified below.
Goto File -> Examples -> ll-teensy-player to see a few examples on how to use.
To use the library, `#include "SdLedsPlayer.h"` in your code, then define an object of type `SdLedsPlayer`.
The object constructor receives number of pixels in each of the 8 strings, and two buffers for led data which the user mush define and supply to the library.

# Main loop
The library let the user handle the main loop, so that extra logic can be done between every frame.

User should call the function `void setup()` once to initialize the object.

Then it should call the function `void load_file(const char *file_name)` to load a new file from SD for display.
The loaded file format should contain:
- The file does not contain any header. only rgb buffers.
- N frames
- Each frame contains (8 * leds_per_strip) pixels. it is the user's responsibility to check that the loaded file was generated with the correct amount of pixels per strip.
- Each pixel should contain 3 bytes of data. the data will be sent to the LED modules in the order found in the file, so user should match the file's RGB ordering to the LED harrdware RGB order.

Then user can apply any required appl-logic, and call the function `bool show_next_frame()` to read next frame from the SD card and send it to the LEDs. Function return true if all goes well, and false in case of error (no file loaded \ end of file \ coruppted data).

To detect if a new file should be loaded, user can call `bool is_file_playing()`. If the function returns `false`, a new file should be loaded to the library.

# Added features
- `bool setBrightness()` function to set global brightness levels (0-255), default is full brightness (255)
- load_file() function now supports loading of new file before previous file finished, replacing it with new file