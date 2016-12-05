#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <SdFatmainpage.h>
#include <SdFatUtil.h>
#include <SdInfo.h>
#include <SdSpi.h>
#include <SdSpiCard.h>

#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial mySerial(7, 8);
long lastTime;
long minute = 0;
void setup()
{
 mySerial.begin(19200);               // the GPRS baud rate   
 Serial.begin(19200);                 // the GPRS baud rate 
 delay(500);
 lastTime = millis();
 initArchiver(53);
 testStats();
}
void loop()
{ 
  if (Serial.available())
      TestServer();
  if (mySerial.available())
   Serial.write(mySerial.read());
  long curTime = millis();
  if (curTime - lastTime >= 6000)
  {
    lastTime = curTime;
    TestServer();
    minute++;
  }

  
}

///TestServer()///
///this function is to send the sensor data to the server
void TestServer()
{
  unsigned long val;
  bool isNull;
  retrieveData(3, minute, &val, &isNull);
  Serial.println(val);
  int _min = minute % 60;
  int _hour = minute / 60;

  
 mySerial.println("AT+CIPSTART=\"tcp\",\"ssh.daveg.work\",\"6003\""); //start up the connection
 delay(2000);
 ShowSerialData();
 mySerial.println("AT+CIPSEND"); //begin sending data to remote server
 delay(4000);
 ShowSerialData();
 mySerial.print("#STA:54321;TM:05/18/2016,");
 if(_hour < 10) 
 mySerial.print("0"); 
 mySerial.print(_hour);
 mySerial.print(":");
 if(_min < 10)
 mySerial.print("0");
 mySerial.print(_min);
 mySerial.print(":00;C:15;V:7.19;PU01:");
 mySerial.print(val);
 mySerial.print(";PU02:00001931;PU03:00000199;PU04:00000247;DI:333300;DO:0000;#");
 delay(500);
 mySerial.println((char)26); //sending
 delay(5000);//waitting for reply, important! the time is base on the condition of internet 
 mySerial.println();
 ShowSerialData();
 mySerial.println("AT+CIPCLOSE");//close the connection
 delay(100);
 ShowSerialData();
}
void ShowSerialData()
{
 while(mySerial.available()!=0)
   Serial.write(mySerial.read());
}
