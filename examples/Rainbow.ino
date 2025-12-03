#include "SdLedsPlayer.h"

const char *files_iter_rr[] = {"1","2","3","4","5","6","7","8","9","10","11","12","13"};

/*
 * SdLedsPlayer is the class that handles reading frames from file on SD card,
 * and writing it to the leds.
 */
SdLedsPlayer sd_leds_player;
int cnt=-1;
bool loopon = true;
void setup() {
  Serial.begin(115200);
  sd_leds_player.setup();
}


void loop() {
 if(!sd_leds_player.is_file_playing()) {
    if (loopon){ cnt=cnt+1;}
    if (cnt>12){cnt=0;}
    sd_leds_player.load_file(files_iter_rr[cnt]);
  }
   delay(40);
  sd_leds_player.show_next_frame();
}
