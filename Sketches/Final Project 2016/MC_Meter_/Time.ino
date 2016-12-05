unsigned long currentTime;
unsigned long lastMSTime;
unsigned long elapsedMilliseconds;
unsigned long lastTimeRequest;
bool timeKnown;

void setCurrentTime(unsigned long time)
{
  currentTime = time;
  timeKnown = true;
  Serial.print("Time is ");
  Serial.println(currentTime);
}

unsigned long getCurrentTime()
{
  return currentTime;
}

void setupTime()
{
  timeKnown = false;
  currentTime = 100;
  lastMSTime = millis();
  secretary->requestTime(0);
  lastTimeRequest = millis();
  elapsedMilliseconds = 0;
}

void loopTime()
{
  unsigned long currentMSTime = millis();
  elapsedMilliseconds += (currentMSTime - lastMSTime);
  lastMSTime = currentMSTime;
  if (!timeKnown && currentMSTime - lastTimeRequest >= 10000)
  {
    lastTimeRequest = currentMSTime;
    secretary->requestTime(0);
  }
  //Serial.println(elapsedMilliseconds);
  lastMSTime = currentMSTime;
  while (elapsedMilliseconds > 60000)
  {
    elapsedMilliseconds -= 60000;
    currentTime++;
    onMinuteElapsedMeter();
  }
}
