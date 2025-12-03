
#include "SdLedsPlayer.h"

// Use this with the Teensy 3.5 & 3.6 & 4.1 SD card
#define SDCARD_CS_PIN BUILTIN_SDCARD

bool SdLedsPlayer::setup()
{
    SDStatus = SD.begin(SDCARD_CS_PIN);
    if (!SDStatus)
    {
        Serial.print("SD card begin() failed using pin: ");
        Serial.println(SDCARD_CS_PIN);
        return false;
    }
    return true;
}

bool SdLedsPlayer::load_file(const char *file_name)
{
    if (!SDStatus)
    {
        Serial.println("SD card not initialized, can't load file");
        return false;
    }
    if (is_file_playing())
    {
        current_file.close();
    }
    current_file = SD.open(file_name);
    if (!(is_file_playing()))
    {
        Serial.println("file open failed");
        return false;
    }
    Serial.println("file open success");

    // Read first 2 bytes which are expected to be the max string length
    uint8_t byte1, byte2;
    if (current_file.read(&byte1, 1) != 1 || current_file.read(&byte2, 1) != 1)
    {
        Serial.println("failed to read first 2 bytes");
        current_file.close();
        return false;
    }

    // Combine the two bytes into a 16-bit unsigned number (little-endian) and store it
    max_string_len = (byte2 << 8) | byte1;
    // Calculate and store total_pixels and bytes_per_frame
    total_pixels = max_string_len * NUM_OF_STRIPS;
    int pixels_in_bytes = total_pixels * CHANNELS_PER_PIXEL;
    bytes_per_frame = TIME_HEADER_SIZE + pixels_in_bytes;

    // Allocate frame buffer based on bytes_per_frame
    // Free existing buffer if it exists
    if (frame_buf != nullptr)
    {
        free(frame_buf);
    }
    frame_buf = (uint8_t *)malloc(bytes_per_frame);
    if (frame_buf == nullptr)
    {
        Serial.println("Failed to allocate frame buffer");
        current_file.close();
        return false;
    }

    // Allocate OctoWS2811 memory and object
    // Free existing LED resources if they exist (for reloading files with different sizes)
    if (leds != nullptr)
    {
        delete leds;
        leds = nullptr;
    }
    if (display_memory != nullptr)
    {
        free(display_memory);
        display_memory = nullptr;
    }
    if (drawing_memory != nullptr)
    {
        free(drawing_memory);
        drawing_memory = nullptr;
    }

    // Allocate display and drawing memory for OctoWS2811
    display_memory = malloc(pixels_in_bytes);
    drawing_memory = malloc(pixels_in_bytes);

    if (display_memory == nullptr || drawing_memory == nullptr)
    {
        Serial.println("Failed to allocate LED memory buffers");
        if (display_memory != nullptr) free(display_memory);
        if (drawing_memory != nullptr) free(drawing_memory);
        display_memory = nullptr;
        drawing_memory = nullptr;
        free(frame_buf);
        frame_buf = nullptr;
        current_file.close();
        return false;
    }

    // Create OctoWS2811 object
    leds = new OctoWS2811(max_string_len, display_memory, drawing_memory, WS2811_GRB | WS2811_800kHz);
    if (leds == nullptr)
    {
        Serial.println("Failed to create OctoWS2811 object");
        free(display_memory);
        free(drawing_memory);
        free(frame_buf);
        display_memory = nullptr;
        drawing_memory = nullptr;
        frame_buf = nullptr;
        current_file.close();
        return false;
    }

    // Initialize the LED strips
    leds->begin();
    Serial.println("OctoWS2811 initialized");

    Serial.print("Max string length: ");
    Serial.println(max_string_len);
    Serial.print("Bytes per frame: ");
    Serial.println(bytes_per_frame);

    return true;
}

bool SdLedsPlayer::is_file_playing()
{
    if (current_file)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SdLedsPlayer::stop_file()
{
    if (is_file_playing())
    {
        current_file.close();
        if (leds != nullptr)
        {
            for (int i = 0; i < total_pixels; i++)
            {
                leds->setPixel(i, 0, 0, 0);
            }
            leds->show();
        }
    }
}

bool SdLedsPlayer::setBrightness(uint8_t brightness)
{
    brightFactor = brightness;
    return true;
}

unsigned long SdLedsPlayer::load_next_frame()
{
    if (!is_file_playing())
    {
        return 0;
    }

    int bytes_read = current_file.read(frame_buf, bytes_per_frame);
    if (bytes_read < 0)
    {
        Serial.println("file read failed");
    }
    if (bytes_read == 0)
    {
        current_file.close();
        return 0;
    }
    if (bytes_read < bytes_per_frame)
    {
        Serial.print("read frame with missing bytes.");
        return 0;
    }
    unsigned long timestamp = ((frame_buf[3] << 24) + (frame_buf[2] << 16) + (frame_buf[1] << 8) + (frame_buf[0]));
    uint8_t r, g, b;
    for (int i = 0; i < total_pixels; i++)
    {
        r = (frame_buf[3 * i + TIME_HEADER_SIZE] * brightFactor) >> 8;
        g = (frame_buf[3 * i + 1 + TIME_HEADER_SIZE] * brightFactor) >> 8;
        b = (frame_buf[3 * i + 2 + TIME_HEADER_SIZE] * brightFactor) >> 8;
        leds->setPixel(i, r, g, b);
    }
    return timestamp;
}

void SdLedsPlayer::show_next_frame()
{
    if (leds != nullptr)
    {
        leds->show();
    }
}
