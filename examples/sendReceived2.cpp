#include <Arduino.h>
#include <ArduinoJson.h>

#define SENDER_TXD_PIN 13
#define RECEIVER_RXD_PIN 12

const int device = 2;
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
  Serial.println("Info: Device: " + String(device));
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
    payload = "{\"message\":\"send data\",\"packages\":\"" + String(packages) + "\",\"device\":\"" + String(device) + "\"}~";
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
      jsonSerialReceived.clear();
      digitalWrite(BUILTIN_LED, HIGH);
      delay(10000);
      digitalWrite(BUILTIN_LED, LOW);
      return;
    }

    Serial.println("Info: " + serialData);

    String message = jsonSerialReceived["message"];
    String packages = jsonSerialReceived["packages"];
    String device = jsonSerialReceived["device"];

    Serial.println("Info: message => " + message);
    Serial.println("Info: packages => " + packages);
    Serial.println("Info: device => " + device);
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

  if (jsonSerialReceived["device"].isNull())
    return false;

  if (jsonSerialReceived["device"] == String(device))
  {
    return false;
  }

  return true;
}
