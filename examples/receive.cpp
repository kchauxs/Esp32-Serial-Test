#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 12
#define RECEIVER_RXD_PIN 13

HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(2048);
String serialData;

bool validateDataSerialReceived(String serialData);
bool validateJsonFromReceivedDataSerial(String serialData);

void setup()
{
  Serial.begin(115200);
  serialSend.begin(115200, SERIAL_8N1, SENDER_TXD_PIN, RECEIVER_RXD_PIN);
  delay(200);
  Serial.println("");
  Serial.println("Received divice!");
}

void loop()
{
  bool isReceivedBySerial = false;
  serialData = "";

  while (serialSend.available())
  {
    char character = (char)serialSend.read();
    if (character == '~')
    {
      isReceivedBySerial = true;
    }
    else if (character != '\n')
    {
      serialData += String(character);
    }
  }

  if (isReceivedBySerial)
  {
    if (!validateDataSerialReceived(serialData))
      return;

    Serial.println("Info: " + serialData);
    Serial.println("");
  }
}

bool validateDataSerialReceived(String serialData)
{
  if (serialData.length() <= 0)
    return false;

  if (!serialData.startsWith("{") && !serialData.endsWith("}"))
    return false;

  return true;
}
