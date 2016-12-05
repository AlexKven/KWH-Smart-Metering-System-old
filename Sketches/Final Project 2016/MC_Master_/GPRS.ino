#include <SPI.h>
#include <String.h>
#include <stdio.h>

#define DATA_LOG            "datalog.txt"
#define MAX_CUSTOMERS       255
#define CC_LENGTH           1
#define AC_LENGTH           3
#define PN_LENGTH           7
#define TOTAL_PN_LENGTH     (CC_LENGTH + AC_LENGTH + PN_LENGTH)
#define DELIMITER           ','
#define METER_NUM_SIZE      3
#define USAGE_SIZE          5
#define MAX_CHAR_SIZE       20

//SoftwareSerial gprsSerial(10, 8);
#define gprsSerial Serial2
String nextMessage = "";
long tme = 0;

void requestMessage(const char *id)
{
  gprsSerial.print("AT+CMGR=");
  gprsSerial.println(id);
}

void deleteMessage(const char *id)
{
  gprsSerial.print("AT+CMGD=");
  gprsSerial.println(id);
}

void SendSMSMessage(String message)
{
  Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  gprsSerial.println("AT+CMGS = \"+12536329681\"");
  delay(100);
  gprsSerial.println(message); //the content of the message
  delay(100);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
}

void RequestTime()
{
  gprsSerial.println("AT+CCLK?");
}

bool testGprs()
{
  gprsSerial.println("AT");
  delay(500);
  bool found = false;
  while (gprsSerial.available())
  {
   if (gprsSerial.read() == 'K');
    found = true;
   delay (100); 
  }
  return found;
}

void gprsPower()
{
 digitalWrite(9,LOW);
 delay(1000);
 digitalWrite(9,HIGH);
 delay(2000);
 digitalWrite(9,LOW);
 delay(3000);
}

void setupGPRS()
{
  gprsSerial.begin(9600); // GPRS shield baud rate  
   pinMode(9, OUTPUT); 
  delay(500);
  if (testGprs())
    Serial.println("GPRS is on.");
  else
  {
    Serial.println("GPRS is off.");
    gprsPower();
  }
  Serial.println("GPRS Setup");
}

void loopGPRS()
{
  while (gprsSerial.available())
  {
    delay(5);
    char c = gprsSerial.read();
    //Serial.write(c); // display the output of the shield
    parseInput(c);
  }
}

