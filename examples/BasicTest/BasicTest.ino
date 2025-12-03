#include "SdLedsPlayer.h"

#define FILE_TO_PLAY "out1"

/*
 * SdLedsPlayer is the class that handles reading frames from file on SD card,
 * and writing it to the leds.
 */
SdLedsPlayer sd_leds_player;

void setup() {
  Serial.begin(115200);
  Serial.println("Started.");
  sd_leds_player.setup();
}

void loop() {

  if(!sd_leds_player.is_file_playing()) {
    sd_leds_player.load_file(FILE_TO_PLAY);
  }

  sd_leds_player.show_next_frame();
}
