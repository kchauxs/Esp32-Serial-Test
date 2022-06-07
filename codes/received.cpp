#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 12
#define RECEIVER_RXD_PIN 13

HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(1024);
String serialData;

bool validateDataSerialReceived(String serialData);
bool validateJsonFromReceivedDataSerial(String serialData);

void setup()
{
  Serial.begin(115200);
  serialSend.begin(115200, SERIAL_8N1, SENDER_TXD_PIN, RECEIVER_RXD_PIN);
  delay(200);
  Serial.println("");
}

void loop()
{
  bool isReceivedBySerial = false;

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
    {
      serialData = "";
      return;
    }

    if (!validateJsonFromReceivedDataSerial(serialData))
    {
      serialData = "";
      return;
    }

    Serial.println("Info: " + serialData);

    String message = jsonSerialReceived["message"];
    String packages = jsonSerialReceived["packages"];

    Serial.println("Info: message => " + message);
    Serial.println("Info: packages => " + packages);
    serialData = "";
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

bool validateJsonFromReceivedDataSerial(String serialData)
{
  deserializeJson(jsonSerialReceived, serialData);

  if (jsonSerialReceived["message"].isNull())
    return false;

  if (jsonSerialReceived["packages"].isNull())
    return false;

  return true;
}
