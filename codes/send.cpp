#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 13
#define RECEIVER_RXD_PIN 12

int packages = 0;
String payload;
HardwareSerial serialSend(1);

void setup()
{
  Serial.begin(115200);
  serialSend.begin(115200, SERIAL_8N1, SENDER_TXD_PIN, RECEIVER_RXD_PIN);
  delay(200);
  Serial.println("");
}

void loop()
{
  delay(10);
  packages++;
  payload = "{\"message\":\"senda data\",\"packages\":\"" + String(packages) + "\"}~";
  serialSend.print(payload);
  // Serial.println(payload);
}