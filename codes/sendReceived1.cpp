#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 12
#define RECEIVER_RXD_PIN 13

const int BOARD = 1;
const int intervalSerial = 50;

unsigned long packages = 0;
unsigned long previousSerialMillis = 0;

String serialData;
String payload;

HardwareSerial serialSend(1);
DynamicJsonDocument jsonSerialReceived(1024);

bool validateDataSerialReceived(String serialData);
bool validateJsonFromReceivedDataSerial(String serialData);
void sendDataBySerial();
void receiveDataBySerial();

void setup()
{
  Serial.begin(115200);
  serialSend.begin(115200, SERIAL_8N1, SENDER_TXD_PIN, RECEIVER_RXD_PIN);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  delay(200);
  Serial.println("BOARD:" + String(BOARD));
  Serial.println("");
}

void loop()
{
  receiveDataBySerial();
  sendDataBySerial();
}

void sendDataBySerial()
{
  long currentmillis = millis();

  if ((currentmillis - previousSerialMillis) > intervalSerial)
  {
    packages++;
    payload = "{\"message\":\"senda data\",\"packages\":\"" + String(packages) + "\",\"board\":\"" + String(BOARD) + "\"}~";
    serialSend.print(payload);
    previousSerialMillis = currentmillis;
  }
}

void receiveDataBySerial()
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
    Serial.println("Info: board => " + packages);
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

  if (jsonSerialReceived["board"] == String(BOARD))
  {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(10000);
    digitalWrite(BUILTIN_LED, LOW);
    return false;
  }
  return true;
}
