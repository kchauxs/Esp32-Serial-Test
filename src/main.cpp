#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 12   // 13
#define RECEIVER_RXD_PIN 13 // 12
#define DEVICE 1            // 2

const int intervalSerial = 100;
unsigned long package = 0;
unsigned long previousSerialMillis = 0;

String serialData;
HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(1024);

bool readSerail(String &serialData);
void receiveDataBySerial();
void sendDataBySerial();
void turnOnLed(int pin, int time = 5000);
bool validateDataSerialReceived(String serialData);
bool validateJsonFromReceivedDataSerial(String serialData);

void turnOnLed(int pin, int time)
{
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);
}

bool readSerail(String &serialData)
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
  return isReceivedBySerial;
}

void receiveDataBySerial()
{
  if (!readSerail(serialData))
    return;

  if (!validateDataSerialReceived(serialData))
    return;

  if (!validateJsonFromReceivedDataSerial(serialData))
  {
    jsonSerialReceived.clear();
    turnOnLed(BUILTIN_LED);
    return;
  }

  String message = jsonSerialReceived["message"];
  String package = jsonSerialReceived["package"];
  String device = jsonSerialReceived["device"];
  jsonSerialReceived.clear();

  Serial.println("");
  Serial.println("Info: " + serialData);
  Serial.println("Info: message => " + message);
  Serial.println("Info: package => " + package);
  Serial.println("Info: device => " + device);
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

  if (jsonSerialReceived["package"].isNull())
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
    package++;
    String toSend = "";

    DynamicJsonDocument jsonSerialSend(2048);
    jsonSerialSend["message"] = "dummy message";
    jsonSerialSend["package"] = package;
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
  delay(200);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  Serial.println("\nInfo: Device: " + String(DEVICE));
}

void loop()
{
  receiveDataBySerial();
  sendDataBySerial();
}