void setup()
{
  Serial.begin(9600);
  gprsSetup();
  if (gprsTestPower())
    Serial.println("GPRS is on.");
  else
  {
    Serial.println("GPRS is off.");
    gprsPower();
  }
  gprsRequestTime();
  delay(2000);
  gprsSendSmsMessage("Hi Alex!", "12536329681");
}

void loop() {
  while (gprsAvailable())
  {
    delay(5);
    char c = gprsRead();
    Serial.write(c); // display the output of the shield
  }
}

//bool fromGprs = true;
//displayReceivedFromMessage(bool _fromGprs)
//{
//  if (fromGprs != _fromGprs)
//  {
//    Serial.println("");
//    Serial.println("");
//    Serial.print("-- From ");
//    Serial.print(_fromGprs ? "GPRS Shield" : "Arduino");
//    Serial.println(" --");
//    fromGprs = _fromGprs;
//  }
//}

