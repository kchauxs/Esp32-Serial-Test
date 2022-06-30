#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 17
#define RECEIVER_RXD_PIN 16
#define DEVICE 1 // 2

const int intervalSerial = 500;
unsigned long package = 0;
unsigned long previousSerialMillis = 0;

HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(1024);

bool validateJsonContent(String serialData);
bool validateString(String serialData);
String readSerail();
void receiveDataBySerial();
void sendDataBySerial();
void showJsonData(String serialData);
void turnOnLed(int time = 2000);

void receiveDataBySerial()
{
  String serialData = readSerail();

  if (!validateString(serialData))
    return;

  if (!validateJsonContent(serialData))
  {
    jsonSerialReceived.clear();
    turnOnLed();
    return;
  }

  showJsonData(serialData);
}

String readSerail()
{
  String serialData = "";
  while (serialSend.available())
  {
    delay(1);
    char character = (char)serialSend.read();
    if (character == '~')
    {
      return serialData;
    }
    else if (character != '\n')
    {
      serialData += String(character);
    }
  }
  return "none";
}

bool validateString(String serialData)
{
  if (serialData.length() == 0)
    return false;

  if (serialData == "none")
    return false;

  if (!serialData.startsWith("{") && !serialData.endsWith("}"))
    return false;

  return true;
}

bool validatvalidateJsonContenteJson(String serialData)
{
  deserializeJson(jsonSerialReceived, serialData);

  if (jsonSerialReceived["messages"].isNull())
    return false;

  if (jsonSerialReceived["package"].isNull())
    return false;

  if (jsonSerialReceived["device"].isNull())
    return false;

  if (jsonSerialReceived["device"] == DEVICE)
    return false;

  return true;
}

void showJsonData(String serialData)
{
  Serial.println("Info: " + serialData);

  String messages = jsonSerialReceived["messages"];
  String package = jsonSerialReceived["package"];
  String device = jsonSerialReceived["device"];

  Serial.println("\nInfo: message: " + messages);
  Serial.println("Info: package: " + package);
  Serial.println("Info: device: " + device);
  jsonSerialReceived.clear();
}

void turnOnLed(int time)
{
  digitalWrite(BUILTIN_LED, HIGH);
  delay(time);
  digitalWrite(BUILTIN_LED, LOW);
}

void sendDataBySerial()
{
  long currentmillis = millis();

  if ((currentmillis - previousSerialMillis) > intervalSerial)
  {
    package++;
    String toSend = "";

    DynamicJsonDocument jsonSerialSend(2048);
    jsonSerialSend["messages"][0]["data"] = "dummy message 0";
    jsonSerialSend["messages"][1]["data"] = "dummy message 1";
    jsonSerialSend["messages"][2]["data"] = "dummy message 2";
    jsonSerialSend["messages"][3]["data"] = "dummy message 3";
    jsonSerialSend["messages"][4]["data"] = "dummy message 4";
    jsonSerialSend["messages"][5]["data"] = "dummy message 5";
    jsonSerialSend["messages"][6]["data"] = "dummy message 6";
    jsonSerialSend["messages"][7]["data"] = "dummy message 7";
    jsonSerialSend["messages"][8]["data"] = "dummy message 8";
    jsonSerialSend["messages"][9]["data"] = "dummy message 9";
    jsonSerialSend["package"] = package;
    jsonSerialSend["device"] = DEVICE;
    serializeJson(jsonSerialSend, toSend);

    toSend += '~';
    serialSend.write(toSend.c_str());
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
  delay(200);
  Serial.println("\nInfo: Device: " + String(DEVICE));
}

void loop()
{
  receiveDataBySerial();
  sendDataBySerial();
}