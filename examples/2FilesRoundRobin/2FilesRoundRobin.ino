#include "SdLedsPlayer.h"

int curr_file_i = 0;
const char *files_iter_rr[] = {"out1", "out2"};

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
    curr_file_i = (curr_file_i + 1) % (sizeof(files_iter_rr) / sizeof(files_iter_rr[0]));
    sd_leds_player.load_file(files_iter_rr[curr_file_i]);
  }

  sd_leds_player.show_next_frame();
}
