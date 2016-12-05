#include <SoftwareSerial.h>
#include <Messenger.h>
#include <Secretary.h>

#define SSerialTxControl 6   //RS485 Direction control
#define SSerialDataEnable 5
//#define serialSource Serial3
SoftwareSerial serialSource(10, 13);

Secretary *secretary;
Messenger *messenger;
byte *curMessage;

void WriteByte(const byte what)
{
  //Serial.write(what);
  serialSource.write(what);
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
  Serial.println("Message Recieved");
  Serial.println(message[9]);
  Serial.println(message[10]);
  byte sender;
  unsigned long time;
  int length;
  unsigned long usage0;
  unsigned long usage1;
  bool twoPoints;
  if (secretary->receiveUsageRequest(message, &time, &length) && !error)
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
  if (secretary->receiveUsageData(message, &sender, &time, &usage0, &usage1, &twoPoints))
  {
    Serial.print(sender);
    Serial.print(" in minute ");
    Serial.print(time);
    Serial.print(" used ");
    Serial.println(usage0);
    if (sender < NUM_METERS)
    {
      currentUsage[sender] = usage0;
      currentIsNull[sender] = error;
    }
    //<NOSD>Serial.println(archiveData(sender, time, usage0));
    if (twoPoints)
    {
      Serial.print(sender);
      Serial.print(" in minute ");
      Serial.print(time + 1);
      Serial.print(" used ");
      Serial.println(usage1);
      //<NOSD>Serial.println(archiveData(sender, time + 1, usage1));
    }
  }
  if (secretary->receiveTimeRequest(message))
  {
    secretary->announceTime(getCurrentTime());
  }
  secretary->handleMessage(error, message, len);
  if (error)
  {
    while (serialSource.read());
  }
}

void setupCommunication()
{
  messenger = new Messenger();
  messenger->initialize(0, 20, 20, WriteByte, AvailableBytes, ReadByte, HandleMessage);
  secretary = new Secretary();
  secretary->initialize(*messenger, SSerialTxControl, SSerialDataEnable, true);
  serialSource.begin(4800);
  curMessage = new byte[20];
}

void loopCommunication()
{
//  if (Serial.available())
//  {
//    String content = "";
//    char character;
//
//    int len = 1;
//  
//    while(Serial.available()) {
//        character = Serial.read();
//        content.concat(character);
//        delay(2);
//        len++;
//    }
//    secretary->requestUsageData(1, content.toInt(), 5);
//    //content.getBytes(curMessage, len);
//    //Serial.write(curMessage, len);
//    //messenger->enqueueMsg(255, curMessage);
//  }
  secretary->loop();
}
