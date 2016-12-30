#include <SoftwareSerial.h>
#define SSerialRX        12
#define SSerialTX        13

#define SSerialTeControl 6
#define SSerialReControl 5

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX
int byteReceived;
int byteSend;

void SetTransmitEnabled(bool val)
{
  if (val)
  {
    digitalWrite(SSerialTeControl, RS485Transmit);
    digitalWrite(SSerialReControl, RS485Transmit);
  }
  else
  {
    digitalWrite(SSerialTeControl, RS485Receive);
    digitalWrite(SSerialReControl, RS485Receive);
  }
}

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("Basic RS-485 Serial test");
  
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTeControl, OUTPUT);    
  pinMode(SSerialReControl, OUTPUT);    

  SetTransmitEnabled(false);
  
  // Start the software serial port, to another device
  RS485Serial.begin(19200);   // set the data rate 

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  if (Serial.available())
  {
    SetTransmitEnabled(true);
    while (Serial.available())
    {
      byteReceived = Serial.read();
      RS485Serial.write(byteReceived);          // Send byte to Remote Arduino
    }
       
    delay(10);
    SetTransmitEnabled(false);  
  }
  
  if (RS485Serial.available())  //Look for data from other Arduino
   {
    digitalWrite(Pin13LED, HIGH);  // Show activity
    byteReceived = RS485Serial.read();    // Read received byte
    Serial.write(byteReceived);        // Show on Serial Monitor
    delay(10);
    digitalWrite(Pin13LED, LOW);  // Show activity   
   }  

}
