#include <SoftwareSerial.h>
#include <Messenger.h>
#include <Secretary.h>

Messenger *messenger;
Secretary *secretary;
byte *curMessage;

#define SSerialTxControl 6   //RS485 Direction control
#define SSerialDataEnable 5
//#define serialSource Serial2;
SoftwareSerial serialSource(12, 13);

void WriteByte(const byte what)
{
  //Serial.write(what);
  serialSource.write(what);
}

void sendUsageData(byte recipient, unsigned long time, unsigned long usage)
{
  secretary->sendUsageData(recipient, time, usage);
}

byte ReadByte()
{
  //Serial.println("Read!");
  return serialSource.read();
}

byte AvailableBytes()
{
  //if (serialSource.available())
  //Serial.println(serialSource.available());
  return serialSource.available();
}

void HandleMessage(bool error, byte *message, int len)
{
  //Serial.println("Message!");
  unsigned long time;
  int length;
  bool on;
  secretary->handleMessage(error, message, len);
  if (secretary->receiveUsageRequest(message, &time, &length))
  {
    Serial.println("Usage Requested");
    for (int i = 0; i < length; i += 2)
    {
      if (i == length - 1 || !isTimeCached(time + i + 1))
      {
        if (isTimeCached(time + i))
        {
          Serial.println("Usage Sent");
          secretary->sendUsageData(0, time + i, getCachedMinuteForTime(time + i));
        }
      }
      else
      {
        if (isTimeCached(time + i))
        {
          Serial.println("Usage Sent");
          secretary->sendUsageData(0, time + i, getCachedMinuteForTime(time + i), getCachedMinuteForTime(time + i + 1));
        }
      }
    }
  }
  if (secretary->receiveTimeAnnouncement(message, &time))
  {
    setCurrentTime(time);
  }
//  if (secretary->receivePowerSetRequest(message, &on))
//  {
//    if (on)
//      digitalWrite(4, HIGH);
//    else
//      digitalWrite(4, LOW);
//  }
}

void setupCommunication()
{
  messenger = new Messenger();
  messenger->initialize(2, 20, 20, WriteByte, AvailableBytes, ReadByte, HandleMessage);
  secretary = new Secretary();
  secretary->initialize(*messenger, SSerialTxControl, SSerialDataEnable, false);
  
  serialSource.begin(4800);
  Serial.println(messenger->getMessengerId());
  curMessage = new byte[20];
}

void loopCommunication()
{
  secretary->loop();
}
