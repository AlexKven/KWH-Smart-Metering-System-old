//Power is approx. 9.7 watts per hertz.
//One tick is approx. 0.0026944444465999994 watt-hours.
byte *recentData;

void setup() {
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  setupCommunication();
  setupTime();
  setupMeter();
  Serial.begin(9600);
  Serial.println("Meter!");
}

void loop() 
{
  loopTime();
  loopMeter();
  loopCommunication();
//  if (Serial.available())
//  {
//    do
//    {
//      Serial.read();
//      delay(100);
//    } while (Serial.available());
//
//    Serial.println("meter");
//    onMinuteElapsedMeter();
//  }
}
