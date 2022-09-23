
#include "SdLedsPlayer.h"

// Use this with the Teensy 3.5 & 3.6 & 4.1 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD

bool SdLedsPlayer::setup() {
  SDStatus = SD.begin(SDCARD_CS_PIN);
  if (!SDStatus) {
    Serial.print("SD card begin() failed using pin: "); Serial.println(SDCARD_CS_PIN);
    return false;
  }
  leds.begin();  
  return true;  
}

bool SdLedsPlayer::load_file(const char *file_name) {
  if (!SDStatus) {
    Serial.println("SD card not initialized, can't load file");
    return false;
  }
  if (is_file_playing()) {
    current_file.close();
  }
  current_file = SD.open(file_name);
  if (!(is_file_playing())) {
    Serial.println("file open failed");
    return false;
  }
  Serial.println("file open success");
  return true;
}

bool SdLedsPlayer::is_file_playing() {
  if (current_file) {
    return true;
  }
  else {
    return false;
  }
}

void SdLedsPlayer::stop_file() {
  if (is_file_playing()) {
    current_file.close();
    for(int i=0; i< total_pixels; i++) {
      leds.setPixel(i, 0, 0, 0);
    }
    leds.show();
  }
}

bool SdLedsPlayer::setBrightness(uint8_t brightness) {
  brightFactor = brightness;
  return true;
}

unsigned long SdLedsPlayer::load_next_frame() {
  if(!is_file_playing()) {
    return 0;
  }
  int bytes_read = current_file.read(frame_buf, bytes_per_frame);
  if (bytes_read < 0) {
    Serial.println("file read failed");
  }  
  if(bytes_read == 0) {
    current_file.close();  
    return 0;
  }
  if(bytes_read < bytes_per_frame) {
    Serial.print("read frame with missing bytes.");
    return 0;
  }
  unsigned long timestamp = ( (frame_buf[3] << 24) 
                   + (frame_buf[2] << 16) 
                   + (frame_buf[1] << 8) 
                   + (frame_buf[0] ));
  uint8_t r,g,b;
  for(int i=0; i< total_pixels; i++) {
    r = (frame_buf[3*i+TIME_HEADER_SIZE] * brightFactor) >> 8;
    g = (frame_buf[3*i+1+TIME_HEADER_SIZE] * brightFactor) >> 8;
    b = (frame_buf[3*i+2+TIME_HEADER_SIZE] * brightFactor) >> 8;
    leds.setPixel(i, r, g, b);
  }
  return timestamp;
}

void SdLedsPlayer::show_next_frame() {
  leds.show();
}
