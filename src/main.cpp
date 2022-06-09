#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 12   // 13
#define RECEIVER_RXD_PIN 13 // 12
#define DEVICE 1            // 2

const int intervalSerial = 100;
unsigned long packages = 0;
unsigned long previousSerialMillis = 0;

String serialData;
HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(1024);

bool readSerail();
void receiveDataBySerial();
void sendDataBySerial();
bool validateDataSerialReceived(String serialData);
bool validateJsonFromReceivedDataSerial(String serialData);

bool readSerail()
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

  return isReceivedBySerial;
}

void receiveDataBySerial()
{
  serialData = "";

  if (readSerail())
  {
    if (!validateDataSerialReceived(serialData))
      return;

    if (!validateJsonFromReceivedDataSerial(serialData))
    {
      jsonSerialReceived.clear();
      digitalWrite(BUILTIN_LED, HIGH);
      delay(10000);
      digitalWrite(BUILTIN_LED, LOW);
      return;
    }

    String message = jsonSerialReceived["message"];
    String packages = jsonSerialReceived["packages"];
    String device = jsonSerialReceived["device"];

    Serial.println("");
    Serial.println("Info: " + serialData);
    Serial.println("Info: message => " + message);
    Serial.println("Info: packages => " + packages);
    Serial.println("Info: device => " + device);
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

  if (jsonSerialReceived["device"].isNull())
    return false;

  if (jsonSerialReceived["device"] == DEVICE)
    return false;

  return true;
}

void sendDataBySerial()
{
  long currentmillis = millis();

  if ((currentmillis - previousSerialMillis) > intervalSerial)
  {
    packages++;
    String toSend = "";

    DynamicJsonDocument jsonSerialSend(2048);
    jsonSerialSend["message"] = "dummy message";
    jsonSerialSend["packages"] = packages;
    jsonSerialSend["device"] = DEVICE;
    serializeJson(jsonSerialSend, toSend);

    toSend += '~';
    serialSend.print(toSend);
    previousSerialMillis = currentmillis;
  }
}

void setup()
{
  Serial.begin(115200);
  serialSend.begin(115200, SERIAL_8N1, SENDER_TXD_PIN, RECEIVER_RXD_PIN);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  delay(200);
  Serial.println("Info: Device: " + String(DEVICE));
  Serial.println("");
}

void loop()
{
  receiveDataBySerial();
  sendDataBySerial();
}