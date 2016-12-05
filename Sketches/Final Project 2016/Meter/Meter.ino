volatile unsigned long LastPulse;
volatile unsigned long PulsePeriod;

void setup() {
  pinMode(2, INPUT);
  Serial.begin(9600);
  while (!Serial) { }
  attachInterrupt(digitalPinToInterrupt(2), pulse, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println(String(LastPulse) + "     " + String(PulsePeriod));
  delay(10);
}

void pulse()
{
  unsigned long curPulse = micros();
  PulsePeriod = curPulse - LastPulse;
  LastPulse = curPulse;
}

