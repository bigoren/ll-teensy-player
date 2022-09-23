#ifndef SD_LEDS_PLAYER_H__
#define SD_LEDS_PLAYER_H__

#include "SD.h"
#include "OctoWS2811.h"


class SdLedsPlayer 
{

public:
SdLedsPlayer(unsigned int leds_per_strip, void *display_memory, void *drawing_memory) :
  total_pixels(leds_per_strip * NUM_OF_STRIPS),
  bytes_per_frame(TIME_HEADER_SIZE + (total_pixels * CHANNELS_PER_PIXEL)),
  leds(leds_per_strip, display_memory, drawing_memory, WS2811_GRB | WS2811_800kHz)
{
  frame_buf = (uint8_t *)malloc(bytes_per_frame);
}

public:
  // call once for initialization
  bool setup();
  // will read the next frame from the loaded file and write it to the leds array
  // returns long timestamp header info on success and 0 in case no frame is available.
  unsigned long load_next_frame();
  // send the leds array to show on the led strings, to be used after load_next_frame().
  void show_next_frame();
  bool setBrightness(uint8_t brightness);

public:
  // read a new file from sd card for led display
  bool load_file(const char *file_name);
  // return true if there is a file loaded, and there are more frames ready for display
  bool is_file_playing();
  // stop the current file from playing
  void stop_file();

private:
  // setup config
  static const int NUM_OF_STRIPS = 8;
  static const int CHANNELS_PER_PIXEL = 3; // DO NOT CHANGE!
  static const int TIME_HEADER_SIZE = 4; // DO NOT CHANGE!
  const int total_pixels;
  const int bytes_per_frame;
  uint8_t brightFactor = 255;

private:
  // OctoWS2811 stuff
  OctoWS2811 leds;

private:
  // Sd stuff
  uint8_t *frame_buf;
  bool SDStatus;
  File current_file;
  
};


#endif // SD_LEDS_PLAYER_H__
