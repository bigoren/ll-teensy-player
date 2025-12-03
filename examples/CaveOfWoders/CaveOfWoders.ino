#include "Adafruit_VL53L0X.h"
#include "SdLedsPlayer.h"

#define FILE_TO_PLAY "1"

/*
 * SdLedsPlayer is the class that handles reading frames from file on SD card,
 * and writing it to the leds.
 */
SdLedsPlayer sd_leds_player;
bool sd_leds_bool = false;


Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int range = -1;

void setup() {
  Serial.begin(115200);
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  Serial.println("Serial Port Started.");
  sd_leds_player.setup();

  Serial.println("Starting VL53L0X boot");
  while (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    delay(1000);
  }

  Serial.println(F("VL53L0X sensor boot done.\n\n"));
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    range = measure.RangeMilliMeter;
    Serial.print("Distance (mm): "); Serial.println(range);
  } else {
    range = -1;
    Serial.println("out of range ");
  }

  if(!sd_leds_player.is_file_playing()) {
    Serial.println("No file is playing, loading new file");
    sd_leds_player.load_file(FILE_TO_PLAY);
  }

  sd_leds_bool = false;
  sd_leds_bool = sd_leds_player.show_next_frame();
  if (!sd_leds_bool) {
    Serial.print("Show frame failed, flag is: ");Serial.println(sd_leds_bool);
  }
}
