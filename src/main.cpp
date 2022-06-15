#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 17
#define RECEIVER_RXD_PIN 16
#define DEVICE 1 // 2

const int intervalSerial = 500;
unsigned long package = 0;
unsigned long previousSerialMillis = 0;

String serialData;
HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(1024);

bool readSerail(String &serialData);
void receiveDataBySerial();
void sendDataBySerial();
void turnOnLed(int pin, int time = 3000);
bool validateDataSerialReceived(String serialData);
bool validateJsonFromReceivedDataSerial(String serialData);

void turnOnLed(int pin, int time)
{
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);
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

void receiveDataBySerial()
{
  String serialData = readSerail();

  if (serialData == "none")
    return;

  if (!validateDataSerialReceived(serialData))
    return;

  if (!validateJsonFromReceivedDataSerial(serialData))
  {
    jsonSerialReceived.clear();
    turnOnLed(BUILTIN_LED);
    return;
  }

  Serial.println("");
  Serial.println("Info: " + serialData);
  Serial.println("");

  String messages = jsonSerialReceived["messages"];
  String package = jsonSerialReceived["package"];
  String device = jsonSerialReceived["device"];
  jsonSerialReceived.clear();

  Serial.println("Info: message => " + messages);
  Serial.println("Info: package => " + package);
  Serial.println("Info: device => " + device);
}

bool validateDataSerialReceived(String serialData)
{
  if (serialData.length() == 0)
    return false;

  if (!serialData.startsWith("{") && !serialData.endsWith("}"))
    return false;

  return true;
}

bool validateJsonFromReceivedDataSerial(String serialData)
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

void sendDataBySerial()
{
  long currentmillis = millis();

  if ((currentmillis - previousSerialMillis) > intervalSerial)
  {
    package++;
    String toSend = "";

    DynamicJsonDocument jsonSerialSend(2048);
    jsonSerialSend["messages"][1]["data"] = "dummy message 1";
    jsonSerialSend["messages"][2]["data"] = "dummy message 2";
    jsonSerialSend["messages"][3]["data"] = "dummy message 3";
    jsonSerialSend["messages"][4]["data"] = "dummy message 4";
    jsonSerialSend["messages"][5]["data"] = "dummy message 5";
    jsonSerialSend["messages"][6]["data"] = "dummy message 6";
    jsonSerialSend["messages"][7]["data"] = "dummy message 7";
    jsonSerialSend["messages"][8]["data"] = "dummy message 8";
    jsonSerialSend["package"] = package;
    jsonSerialSend["device"] = 2;
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

  Serial.println("\nInfo: Device: " + String(DEVICE));
}

void loop()
{
  receiveDataBySerial();
  sendDataBySerial();
}