/**
  Solaroid - for Seeeduino XIAO part

  Take a photo after boot
  and send it through serial communication
*/

#include <SPI.h>
#include "epd4in2.h"
#include "imagedata.h"

#define COLORED 0
#define UNCOLORED 1

#define LENGTH 15000
uint8_t frame_buffer[LENGTH];

#include "ArduinoLowPower.h"

#define CAM_POWER_PIN 6
#define INTERRUPT_PIN 1

Epd epd;

bool sleeping = true;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(1000000);

  pinMode(CAM_POWER_PIN, OUTPUT);
  digitalWrite(CAM_POWER_PIN, LOW);

  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(INTERRUPT_PIN, take_a_photo, FALLING);

  if (epd.Init() != 0)
  {
    Serial.println("e-Paper init failed");
    return;
  }
}

void take_a_photo()
{
  Serial.println("take a photo");
  digitalWrite(CAM_POWER_PIN, HIGH);
  sleeping = false;
}

void loop()
{
  while (Serial1.available())
  {
    Serial1.readBytes(frame_buffer, LENGTH);

    Serial.println("received shot");
    sleeping = true;

    digitalWrite(CAM_POWER_PIN, LOW);
    epd.Init();
    epd.DisplayFrame(frame_buffer);
  }

  if (sleeping)
  {
    epd.Sleep();
    LowPower.sleep();
  }
}
