#include <SoftwareSerial.h>
#include <Messenger.h>
#include <Secretary.h>
#include <SPI.h>
#include <SdFat.h>
//#include <Archiver.cpp>
//#include <Archiver.h>

#define SSerialTxControl 6   //RS485 Direction control
#define SSerialDataEnable 5
//#define serialSource Serial2;
SoftwareSerial serialSource(8, 9);

Secretary *secretary;
Messenger *messenger;
byte *curMessage;
File file1;
File file2;
File myFile;
//SdFat sd;
//SdFile sdFile;

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

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
  //Serial.println(message[9]);
  byte sender;
  unsigned long t;
  unsigned long usage0;
  unsigned long usage1;
  switch (message[9])
  {
    case 1:
      sender = message[10];
      t = 0;
      for (int i = 0; i < 4; i++)
      {
        t = t << 8;
        t += message[11 + i];
      }
      usage0 = 0;
      for (int i = 0; i < 4; i++)
      {
        usage0 = usage0 << 8;
        usage0 += message[15 + i];
      }
      Serial.print(sender);
      Serial.print(" in minute ");
      Serial.print(t);
      Serial.print(" used ");
      Serial.println(usage0);
    break;
    case 2:
      sender = message[10];
      t = 0;
      for (int i = 0; i < 3; i++)
      {
        t = t << 8;
        t += message[11 + i];
      }
      t *= 2;
      usage0 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage0 = usage0 << 8;
        usage0 += message[14 + i];
      }
      usage1 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage1 = usage1 << 8;
        usage1 += message[17 + i];
      }
      Serial.print(sender);
      Serial.print(" in minute ");
      Serial.print(t);
      Serial.print(" used ");
      Serial.println(usage0);
      Serial.print(sender);
      Serial.print(" in minute ");
      Serial.print(t + 1);
      Serial.print(" used ");
      Serial.println(usage1);
    break;
    case 3:
      sender = message[10];
      t = 0;
      for (int i = 0; i < 3; i++)
      {
        t = t << 8;
        t += message[11 + i];
      }
      t *= 2;
      t++;
      usage0 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage0 = usage0 << 8;
        usage0 += message[14 + i];
      }
      usage1 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage1 = usage1 << 8;
        usage1 += message[17 + i];
      }
      Serial.print(sender);
      Serial.print(" in minute ");
      Serial.print(t);
      Serial.print(" used ");
      Serial.println(usage0);
      Serial.print(sender);
      Serial.print(" in minute ");
      Serial.print(t + 1);
      Serial.print(" used ");
      Serial.println(usage1);
    break;
    case 4:
      sender = message[10];
      t = 0;
      for (int i = 0; i < 4; i++)
      {
        t = t << 8;
        t += message[11 + i];
      }
      usage0 = 0;
      for (int i = 0; i < 4; i++)
      {
        usage0 = usage0 << 8;
        usage0 += message[15 + i];
      }
      Serial.print(sender);
      Serial.print(" in hour ");
      Serial.print(t);
      Serial.print(" used ");
      Serial.println(usage0);
    break;
    case 5:
      sender = message[10];
      t = 0;
      for (int i = 0; i < 3; i++)
      {
        t = t << 8;
        t += message[11 + i];
      }
      t *= 2;
      usage0 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage0 = usage0 << 8;
        usage0 += message[14 + i];
      }
      usage1 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage1 = usage1 << 8;
        usage1 += message[17 + i];
      }
      Serial.print(sender);
      Serial.print(" in hour ");
      Serial.print(t);
      Serial.print(" used ");
      Serial.println(usage0);
      Serial.print(sender);
      Serial.print(" in hour ");
      Serial.print(t + 1);
      Serial.print(" used ");
      Serial.println(usage1);
    break;
    case 6:
      sender = message[10];
      t = 0;
      for (int i = 0; i < 3; i++)
      {
        t = t << 8;
        t += message[11 + i];
      }
      t *= 2;
      t++;
      usage0 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage0 = usage0 << 8;
        usage0 += message[14 + i];
      }
      usage1 = 0;
      for (int i = 0; i < 3; i++)
      {
        usage1 = usage1 << 8;
        usage1 += message[17 + i];
      }
      Serial.print(sender);
      Serial.print(" in hour ");
      Serial.print(t);
      Serial.print(" used ");
      Serial.println(usage0);
      Serial.print(sender);
      Serial.print(" in hour ");
      Serial.print(t + 1);
      Serial.print(" used ");
      Serial.println(usage1);
    break;
  }
  secretary->handleMessage(error, message, len);
}

void initExternalLibraries()
{
  messenger = new Messenger();
  messenger->initialize(0, 20, 20, WriteByte, AvailableBytes, ReadByte, HandleMessage);
  secretary = new Secretary();
  secretary->initialize(*messenger, SSerialTxControl, SSerialDataEnable, true);
  initArchiver(53);
}

void setup() {
  initExternalLibraries();
  Serial.begin(9600);
  serialSource.begin(4800);
  Serial.print("My ID = ");
  Serial.println(0);
  curMessage = new byte[20];
  //ConfiguresdCard();
  //Testsd();
  //Readsd();
  //Testsd();
  //CreateArchiveFile();
  testLifetime();
  Serial.println("Done");
}

void loop() {
  if (Serial.available())
  {
    String content = "";
    char character;

    int len = 1;
  
    while(Serial.available()) {
        character = Serial.read();
        content.concat(character);
        delay(2);
        len++;
    }
    secretary->requestUsageData(1, content.toInt(), 1, true);
    //content.getBytes(curMessage, len);
    //Serial.write(curMessage, len);
    //messenger->enqueueMsg(255, curMessage);
  }
  secretary->loop();
}
