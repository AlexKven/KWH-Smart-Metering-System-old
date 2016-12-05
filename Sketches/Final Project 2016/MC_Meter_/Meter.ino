#include <EEPROM.h>

volatile unsigned long totalPulses;
unsigned long timeCountingPulses;
unsigned long pulsesLastMinute;

const int NUM_CACHED_HOURS = 2;
//const bool CLEAR_EEPROM = true;
const double WATTHOURS_PER_PULSE = 0.0026944444465999994;
const double WATTHOURS_PER_UNIT = 0.0019073486;
const double UNITS_PER_PULSE = 1.412664914;
byte *cachedMinutes;
//unsigned int currentMinuteIndex;
int numMinutes;
unsigned int minuteStartIndex;

void moveNextMinute()
{
  for (int i = NUM_CACHED_HOURS * 60 * 3 - 1; i > 2; i--)
  {
    cachedMinutes[i] = cachedMinutes[i - 3];
  }
  if (numMinutes < NUM_CACHED_HOURS * 60)
    numMinutes++;
}

unsigned long getCachedMinuteIndex(int index)
{
  unsigned long result = 0;
  for (int j = 0; j < 3; j++)
  {
    result = result << 8;
    result += cachedMinutes[index * 3 + j];
//    Serial.println(index * 3);
//    Serial.println(cachedMinutes[index * 3 + j]);
  }
  return result;
}

void setCachedMinuteIndex(int index, unsigned long data)
{
  for (int i = 0; i < 3; i++)
  {
    cachedMinutes[index * 3 + i] = (data >> 16 - 8 * i);
//    Serial.println(index * 3);
//    Serial.println(cachedMinutes[index * 3 + j]);
  }
}
//
//unsigned long getCachedMinute(unsigned int index)
//{
//  unsigned int actualIndex = (minuteStartIndex + index) % (NUM_CACHED_HOURS * 60);
//  unsigned long result = 0;
//  for (int j = 0; j < 3; j++)
//  {
//    result = result << 8;
//    result += cachedMinutes[3 * actualIndex + j];
//  }
//  return result;
//}

unsigned long getCachedMinuteForTime(unsigned long time)
{
  unsigned int diff = (getCurrentTime() - time);
  return getCachedMinuteIndex(diff);
}
//
//void setCachedMinute(unsigned int index, unsigned long data)
//{
//  unsigned int actualIndex = (minuteStartIndex + index) % (NUM_CACHED_HOURS * 60);
//  for (int i = 0; i < 3; i++)
//  {
//    cachedMinutes[3 * actualIndex + i] = (data >> 16 - 8 * i);
//  }
//}

//void setCachedMinuteEEPROM(unsigned int index, unsigned long data)
//{
//  unsigned int actualIndex = (minuteStartIndex + index) % (NUM_CACHED_HOURS * 60);
//  for (int i = 0; i < 3; i++)
//  {
//    EEPROM[6 + 3 * actualIndex + i] = (data >> 16 - 8 * i);
//  }
//  EEPROM[0] = (numMinutes >> 8);
//  EEPROM[1] = (numMinutes);
//  EEPROM[2] = (minuteStartIndex >> 8);
//  EEPROM[3] = (minuteStartIndex);
//}

long safelyGetTotalPulses()
{
  long result;
  cli();
  result = totalPulses;
  sei();
  return result;
}

void safelyIncrementTotalPulses()
{
  cli();
  totalPulses++;
  sei();
}

void safelyZeroTotalPulses()
{
  cli();
  totalPulses = 0;
  sei();
  timeCountingPulses = micros();
}

void pulse()
{
  safelyIncrementTotalPulses();
}

void readStateFromEEPROM()
{
//  numMinutes = 0;
//  for (int j = 0; j < 2; j++)
//  {
//    numMinutes = numMinutes << 8;
//    numMinutes += EEPROM[0 + j];
//  }
//  minuteStartIndex = 0;
//  for (int j = 0; j < 2; j++)
//  {
//    minuteStartIndex = minuteStartIndex << 8;
//    minuteStartIndex += EEPROM[2 + j];
//  }
//  for (int i = 0; i < NUM_CACHED_HOURS * 60 * 3; i++)
//  {
//    cachedMinutes[i] = EEPROM[6 + i];
//  }
}

void clearEEPROM()
{
//  for (int i = 0 ; i < EEPROM.length() ; i++) {
//     EEPROM.write(i, 0);
//   }
}

void onMinuteElapsedMeter()
{
  Serial.println("Minute passed.");
  moveNextMinute();
  Serial.println("Move next minute.");
  double wattHours;
  unsigned long formattedWattHours;
  unsigned long pulses;
//  if (numMinutes == NUM_CACHED_HOURS * 60)
//      minuteStartIndex = (minuteStartIndex + 1) % (NUM_CACHED_HOURS * 60);
//    else
//      numMinutes++;
    pulses = safelyGetTotalPulses();
    unsigned long pulseDiff = pulses - pulsesLastMinute;
    //timeLastMinute = _millis;
    //wattHours = (double)pulseDiff * WATTHOURS_PER_PULSE;
    formattedWattHours = (double)pulseDiff * UNITS_PER_PULSE;
    Serial.print(" Pulses: ");
    Serial.print(pulseDiff);
//    Serial.print(" Watthours: ");
//    Serial.print(wattHours);
    Serial.print(" Formatted Watthours: ");
    Serial.println(formattedWattHours);
    setCachedMinuteIndex(0, formattedWattHours);
    Serial.print(" From RAM: ");
    Serial.println(getCachedMinuteIndex(0));
    //secretary->sendUsageData(0, getCurrentTime(), formattedWattHours);
//    if (NUM_CACHED_HOURS == 2)
//      setCachedMinuteEEPROM(numMinutes - 1, formattedWattHours);
    pulsesLastMinute = pulses;
  
    for (int i = 0; i < numMinutes; i++)
    {
      unsigned long time = getCurrentTime() - i;
      formattedWattHours = getCachedMinuteForTime(time);
      wattHours = (double)formattedWattHours * WATTHOURS_PER_UNIT;
      Serial.print("Minute: ");
      Serial.print(time);
      Serial.print(" Energy: ");
      Serial.println(formattedWattHours);
    }
}
  

bool isTimeCached(unsigned long time)
{
  int diff = (getCurrentTime() - time);
  return diff < numMinutes && diff >= 0;
}

void setupMeter()
{
  cachedMinutes = new byte[60 * 3 * NUM_CACHED_HOURS];
//  if (NUM_CACHED_HOURS == 2)
//  {
//    if (CLEAR_EEPROM)
//    {
//      clearEEPROM();
//      numMinutes = 0;
//      minuteStartIndex = 0;
//    }
//    else
//      readStateFromEEPROM();
//  }
  numMinutes = 0;
  minuteStartIndex = 0;
  pulsesLastMinute = safelyGetTotalPulses();
  attachInterrupt(digitalPinToInterrupt(2), pulse, RISING);
  safelyZeroTotalPulses();
}

void loopMeter()
{
  unsigned long _millis = millis();
  unsigned long _micros = micros();
  unsigned long pulses;
  
  if (Serial.available())
  {
    while (Serial.available())
    {
      Serial.read();
    }
//    String content = "";
//    char character;
//
//    int len = 1;
//  
//    while(Serial.available()) {
//        character = Serial.read();
//        content.concat(character);
//        delay(2);
//        len++;
//    }
//    content.getBytes(curMessage, len);
//    messenger->enqueueMsg(255, curMessage);
    pulses = safelyGetTotalPulses();
    unsigned long t = _micros - timeCountingPulses;
    Serial.print(pulses);
    Serial.print(" pulses in ");
    Serial.print(t);
    Serial.println(" microseconds.");
  }
    
}
