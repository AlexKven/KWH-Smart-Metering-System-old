//Using a SIM900 GRPS shield
//Assumed setup: software power-on is enabled and baud rate is 9600
#define gprsSerial Serial2

void gprsSetup()
{
  gprsSerial.begin(9600);
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

bool gprsTestPower()
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

void gprsRequestTime()
{
  gprsSerial.println("AT+CCLK?");
}

void gprsSendSmsMessage(String message, String number)
{
  Serial.println("Sending Text...");
  gprsSerial.println("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  gprsSerial.print("AT+CMGS = \"+");
  gprsSerial.print(number);
  gprsSerial.println("\"");
  delay(100);
  gprsSerial.println(message); //the content of the message
  delay(100);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
}

int gprsAvailable()
{
  return gprsSerial.available();
}

int gprsRead()
{
  return gprsSerial.read();
}

