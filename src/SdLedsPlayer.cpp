
#include "SdLedsPlayer.h"

void SdLedsPlayer::setup() {
  leds.begin();    
}

void SdLedsPlayer::load_file(const char *file_name) {
  if (!sd.begin()) {
    sd.initErrorHalt("SdFatSdioEX begin() failed");
  }
  sd.chvol();
  if (is_file_playing()) {
    current_file.close();
  }
  if (!current_file.open(file_name, O_RDONLY)) {
    sd.errorHalt("open failed");
  }
  Serial.println("open success");  
}

bool SdLedsPlayer::is_file_playing() {
  return current_file.isOpen();
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
    sd.errorHalt("read failed");
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
