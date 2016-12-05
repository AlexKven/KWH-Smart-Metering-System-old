volatile unsigned long LowTime = 0;
volatile unsigned long HighTime = 0;
volatile unsigned long PulseTime = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), stateChange, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long seconds = PulseTime / 1000;
  displayNumber((int)seconds);
//  if (digitalRead(1) == LOW)
//    displayNumber(5);
//  else
//   displayNumber(7);
  delay(100);
}

void displayNumber(int num)
{
  
  if (num % 2 == 1)
    { digitalWrite(9, HIGH); }
  else
    { digitalWrite(9, LOW);}
    
  if (num / 2 % 2 == 1)
    {digitalWrite(10, HIGH);}
  else
    {digitalWrite(10, LOW);}
    
  if (num / 4 % 2 == 1)
    {digitalWrite(11, HIGH);}
  else
    {digitalWrite(11, LOW);}
    
  if (num / 8 % 2 == 1)
    {digitalWrite(12, HIGH);}
  else
    {digitalWrite(12, LOW);}
}

void stateChange()
{
  if (digitalRead(2) == LOW)
    LowTime = millis();
  else
  {
    HighTime = millis();
    PulseTime = HighTime - LowTime;
  }
}

