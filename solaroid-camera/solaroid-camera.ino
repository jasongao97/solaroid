/**
  Solaroid - for ESP32-CAM part

  Arranged code By Frenoy Osburn
  YouTube Video: https://youtu.be/KEU2GTS1YlQ
  BnBe Post: https://www.bitsnblobs.com/digital-still-image-camera---esp32-cam

  https://github.com/eloquentarduino/EloquentArduino
*/

#define CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"
#include "camera_pins.h"

#define FRAME_SIZE FRAMESIZE_HVGA
#define WIDTH 480
#define HEIGHT 320

#define W 400
#define H 300
uint8_t frame_buffer[W * H / 8];

#define RXD 13
#define TXD 12

void setup()
{
  Serial.begin(115200);
  Serial2.begin(1000000, SERIAL_8N1, RXD, TXD);

  if (!setup_camera()) {
    Serial.println("ERR INIT");
    return;
  }

  Serial.println("OK");

  if (!capture_still()) {
    Serial.println("ERR CAPTURE");
    return;
  }

  Serial.println("TRANSFERING");
  Serial2.write(frame_buffer, W * H / 8);
  Serial.println("DONE!");
}

void loop() {}

bool setup_camera()
{
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAME_SIZE;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  return esp_camera_init(&config) == ESP_OK;
}

bool capture_still()
{
  camera_fb_t *frame = esp_camera_fb_get();

  if (!frame) return false;

  // dithering
  for (int y = 0; y < HEIGHT - 1; y++) {
    for (int x = 1; x < WIDTH - 1; x++) {
      uint8_t pixel = frame->buf[y * WIDTH + x];
      uint8_t newPix = pixel > 127 ? 255 : 0;
      frame->buf[y * WIDTH + x] = newPix;

      // calculate pixel error
      float errPix = pixel - newPix;

      int index = y * WIDTH + x + 1;
      float p = frame->buf[index];
      p += errPix * 7 / 16;
      frame->buf[index] = p;

      index = (y + 1) * WIDTH + x - 1;
      p = frame->buf[index];
      p += errPix * 3 / 16;
      frame->buf[index] = p;

      index = (y + 1) * WIDTH + x;
      p = frame->buf[index];
      p += errPix * 5 / 16;
      frame->buf[index] = p;

      index = (y + 1) * WIDTH + x + 1;
      p = frame->buf[index];
      p += errPix * 1 / 16;
      frame->buf[index] = p;
    }
  }

  // process the photo
  for (int x = 0; x < W; x++) {
    for (int y = 0; y < H; y++) {
      uint8_t offsetX = (WIDTH - W) / 2;
      uint8_t offsetY = (HEIGHT - H) / 2;
      uint8_t pixel = frame->buf[(H - 1 - y + offsetY) * WIDTH + W - 1 - x + offsetX];

      int origin = y * W + x;
      int pos = origin % 8;
      int index = (origin - pos) / 8;

      if (pixel > 127) {
        frame_buffer[index] += (0x80 >> pos);
      }
    }
  }

  //  Serial2.write(frame->buf, frame->len);
  esp_camera_fb_return(frame);

  return true;
}
