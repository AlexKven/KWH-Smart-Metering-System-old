volatile int numPulses;
unsigned long _lastTime;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) { }
  attachInterrupt(digitalPinToInterrupt(3), pulse, RISING);
  _lastTime = micros();
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
}

void loop() 
{
  int pulses;
  unsigned long curTime = micros();
  unsigned long lastTime = _lastTime;
  _lastTime = curTime;
  noInterrupts();
  pulses = numPulses;
  numPulses = 0;
  interrupts();
  double timeDiff = (double)(curTime - lastTime) / 1000000.0;
  double freq = (double)pulses / timeDiff;
  Serial.print("Frequency is ");
  Serial.print(freq, 8);
  Serial.println(" hertz.");
  //Adjust delay between readings.
  delay(500);
}

void pulse()
{
  noInterrupts();
  numPulses++;
  interrupts();
}

