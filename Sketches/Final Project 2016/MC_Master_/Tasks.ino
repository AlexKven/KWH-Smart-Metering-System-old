const int MAX_DATA_REQUESTED_SIMUL = 2;

unsigned long timeLastMeterPoll;

void setupTasks()
{
  timeLastMeterPoll = getCurrentTime();
}

void loopTasks()
{
//  delay(5);
//  Serial.print(getCurrentTime());
//  Serial.print(" - ");
//  Serial.print(timeLastMeterPoll);
//  Serial.print(" = ");
//  Serial.println(getCurrentTime() - timeLastMeterPoll);
  if (getCurrentTime() - timeLastMeterPoll >= METER_POLL_INTERVAL_MINS)
  {
    unsigned long start;
    int len;
    if (getCurrentTime() - timeLastMeterPoll <= MAX_DATA_REQUESTED_SIMUL)
    {
      start = getCurrentTime();
      len = 1;
    }
    else
    {
      start = getCurrentTime() - MAX_DATA_REQUESTED_SIMUL - 1;
      len = MAX_DATA_REQUESTED_SIMUL;
    }
    for (int i = 0; i < NUM_METERS; i++)
    {
      Serial.println("Data Requested");
      secretary->requestUsageData(i, start, len);
    }
    timeLastMeterPoll = getCurrentTime();
  }
}
