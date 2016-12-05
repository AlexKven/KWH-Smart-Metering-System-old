unsigned long currentTime;
unsigned long lastMSTime;
unsigned long elapsedMilliseconds;
unsigned long lastTimeRequest;
bool timeKnown;

void setCurrentTime(unsigned long time)
{
  currentTime = time;
  timeKnown = true;
  Serial.print("Current time is: ");
  Serial.println(time);
//  archiveData(3, 5, currentTime - 5);
//  unsigned long dat;
//  bool nul;
//    retrieveData(3, 5, &dat, &nul);
//    Serial.print(3);
//    Serial.print(", ");
//    Serial.print(5);
//    Serial.print(": ");
//    
//    if (nul)
//      Serial.println("Null");
//    else
//      Serial.println(dat);
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
  RequestTime();
  lastTimeRequest = millis();
  elapsedMilliseconds = 0;
}

void loopTime()
{
  unsigned long currentMSTime = millis();
  elapsedMilliseconds += (currentMSTime - lastMSTime);
  lastMSTime = currentMSTime;
  if (currentMSTime >= lastTimeRequest + (timeKnown ? (unsigned long)1440 * (unsigned long)60000 : (unsigned long)10000))
  {
    lastTimeRequest = currentMSTime;
    RequestTime();
  }
  lastMSTime = currentMSTime;
  while (elapsedMilliseconds > 60000)
  {
    elapsedMilliseconds -= 60000;
    currentTime++;
    onMinuteElapsedMeter();
    //serverOnMinutePassed();
  }
}
