/* <NOSD> #include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <SdFatmainpage.h>
#include <SdFatUtil.h>
#include <SdInfo.h>
#include <SdSpi.h>
#include <SdSpiCard.h>

SdFat sd;
SdFile sdFile;
bool readFile;
char fileName[19];
char curLine[11];
char curFileName[19];
char *lifetimeFileName = "lifetime.txt";
const double WATTHOURS_PER_UNIT = 0.0019073486;
byte pendingLifetimeCommitUser;
long pendingLifetimeCommit;

unsigned long lastFileAccessTime;

void intToCurLine(unsigned long val)
{
  unsigned long curDigit;
  for (int i = 9; i >= 0; i--)
  {
    curDigit = val % 10;
    curLine[i] = curDigit + '0';
    val -= curDigit;
    val /= 10;
  }
}

bool getHourlyStatsWH(byte id, unsigned long day, int hour, double *_total, double *_minimum, double *_maximum, double *_mean, double *_stdDev)
{
  unsigned long total;
  unsigned long minimum;
  unsigned long maximum;
  unsigned long mean;
  unsigned long stdDev;
  bool result = retrieveStats(id, day * 1440 + hour * 60, 60, &total, &minimum, &maximum, &mean, &stdDev);
  *_total = total * WATTHOURS_PER_UNIT;
  *_minimum = minimum * WATTHOURS_PER_UNIT;
  *_maximum = maximum * WATTHOURS_PER_UNIT;
  *_mean = mean * WATTHOURS_PER_UNIT;
  *_stdDev = stdDev * WATTHOURS_PER_UNIT;
  return result;
}

bool retrieveStats(byte id, unsigned long startTime, int numMinutes, unsigned long *_total, unsigned long *_minimum, unsigned long *_maximum, unsigned long *_mean, unsigned long *_stdDev)
{
  unsigned long total = 0;
  unsigned long minimum = total - 1;
  unsigned long maximum = 0;
  unsigned long mean;
  unsigned long variance = 0;
  unsigned long value;
  bool isNull;
  bool result = false;
  int sampleSize = numMinutes;
  for (int i = 0; i < numMinutes; i++)
  {
    retrieveData(id, startTime + i, &value, &isNull);
    if (isNull)
    {
      result = true;
      sampleSize--;
    }
    else
    {
      total += value;
      if (value < minimum)
        minimum = value;
      if (value > maximum)
        maximum = value;
    }
  }
  mean = total / sampleSize;
  *_total = total;
  *_minimum = minimum;
  *_maximum = maximum;
  *_mean = mean;
  for (int i = 0; i < numMinutes; i++)
  {
    retrieveData(id, startTime + i, &value, &isNull);
    if (!isNull)
    {
      variance += (value - mean) * (value - mean);
    }
  }
  variance /= result ? (sampleSize - 1) : sampleSize;
  *_stdDev = sqrt(variance);
}

void setCurFileName(byte id, unsigned long day)
{
  curFileName[3] = '-';
  curFileName[6] = '-';
  curFileName[9] = '-';
  curFileName[14] = '.';
  curFileName[15] = 't';
  curFileName[16] = 'x';
  curFileName[17] = 't';
  curFileName[18] = '\0';
  int year = 2016;
  while (day > 1461)
  {
    day -= 1461;
    year += 4;
  }
  while (day > (year % 4 == 0 ? 366 : 365))
  {
    day -= (year % 4 == 0 ? 366 : 365);
    year++;
  }
  if (day < 31)
  {
    curFileName[4] = '0';
    curFileName[5] = '1';
    day++;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28))
  {
    curFileName[4] = '0';
    curFileName[5] = '2';
    day += 1 - 31;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31)
  {
    curFileName[4] = '0';
    curFileName[5] = '3';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28);
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30)
  {
    curFileName[4] = '0';
    curFileName[5] = '4';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31)
  {
    curFileName[4] = '0';
    curFileName[5] = '5';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31 + 30)
  {
    curFileName[4] = '0';
    curFileName[5] = '6';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31 + 30 + 31)
  {
    curFileName[4] = '0';
    curFileName[5] = '7';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31 - 30;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31 + 30 + 31 + 31)
  {
    curFileName[4] = '0';
    curFileName[5] = '8';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31 - 30 - 31;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31 + 30 + 31 + 31 + 30)
  {
    curFileName[4] = '0';
    curFileName[5] = '9';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31 - 30 - 31 - 31;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31)
  {
    curFileName[4] = '1';
    curFileName[5] = '0';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31 - 30 - 31 - 31 - 30;
  }
  else if (day < 31 + (year % 4 == 0 ? 29 : 28) + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
  {
    curFileName[4] = '1';
    curFileName[5] = '1';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31 - 30 - 31 - 31 - 30 - 31;
  }
  else
  {
    curFileName[4] = '1';
    curFileName[5] = '2';
    day += 1 - 31 - (year % 4 == 0 ? 29 : 28) - 31 - 30 - 31 - 30 - 31 - 31 - 30 - 31 - 30;
  }
  curFileName[0] = id / 100 + '0';
  curFileName[1] = (id % 100) / 10 + '0';
  curFileName[2] = (id % 10) + '0';
  curFileName[7] = day / 10 + '0';
  curFileName[8] = day % 10 + '0';
  curFileName[10] = year / 1000 + '0';
  curFileName[11] = (year % 1000) / 100 + '0';
  curFileName[12] = (year % 100) / 10 + '0';
  curFileName[13] = (year % 10) + '0';
}

void closeFile()
{
  if (sdFile.isOpen())
  {
    sdFile.flush();
    while (!sdFile.close())
    {
      Serial.println("Close Error");
      delay(500);
    }
    Serial.println("Close!");
    if (pendingLifetimeCommitUser != 255 && pendingLifetimeCommit != 0)
    {
//      Serial.print("Added to archive: ");
//      Serial.println(pendingLifetimeCommit);
      addToLifetime(pendingLifetimeCommitUser, pendingLifetimeCommit);
      pendingLifetimeCommit = 0;
      pendingLifetimeCommitUser = 255;
    }
  }
}

bool openFile(char *_fileName, bool _readFile, bool (createFileFunc)(char *fn))
{
//  Serial.println(strcmp(fileName, _fileName));
//  Serial.println("");
  Serial.print("Open ");
  Serial.print(_fileName);
  if ((strcmp(fileName, _fileName) == 0) && (!readFile || _readFile) && sdFile.isOpen())
  {
    return true;
  }
  closeFile();
  for (int i = 0; i < 19; i++)
  {
    fileName[i] = _fileName[i];
  }
  readFile = _readFile;
  //Serial.print(fileName);
  int tries = 0;
//  if (!sdFile.exists(fileName))
//      createFileFunc(fileName);
//  while (!sdFile.open(fileName, readFile ? O_READ : O_RDWR))
//  {
//    Serial.print("Failed ");
//    Serial.println(fileName);
//    delay(200);
//    tries++;
//    if (tries % 3 == 0)
//      initArchiver(53);
//  }
//    Serial.print("Opened ");
//    Serial.println(fileName);
  if (!sdFile.open(fileName, readFile ? O_READ : O_RDWR))
  {
    Serial.print("Couldn't open ");
    Serial.println(fileName);
    if (readFile)
      return false;
    else
      createFileFunc(fileName);
  }
  else
    return true;
  return sdFile.open(fileName, readFile ? O_READ : O_RDWR);
}

bool openLifetimeFile(bool _readFile)
{
  closeFile();
  if (!sdFile.open(lifetimeFileName, _readFile ? O_READ : O_RDWR))
  {
    if (_readFile)
      return false;
    else
      createLifetimeFile();
  }
  else
    return true;
  return sdFile.open(lifetimeFileName, _readFile ? O_READ : O_RDWR);
}

unsigned long getNextULong()
{
  unsigned long value = 0;
  char chr;
  for (int i = 0; i < 10; i++)
  {
    chr = sdFile.read();
    value *= 10;
    value += (chr - '0');
  }
  return value;
}

bool addToLifetime(byte id, long amount)
{
  unsigned long high = 0;
  unsigned long low = 0;
//  curFileName[0] = 'l';
//  curFileName[1] = 'i';
//  curFileName[2] = 'f';
//  curFileName[3] = 'e';
//  curFileName[4] = 't';
//  curFileName[5] = 'i';
//  curFileName[6] = 'm';
//  curFileName[7] = 'e';
//  curFileName[8] = '.';
//  curFileName[9] = 't';
//  curFileName[10] = 'x';
//  curFileName[11] = 't';
//  curFileName[12] = '\0';
  if (!openLifetimeFile(false))
    return false;
//  sdFile.seekSet(id * 21);
//  for (int i = 0; i < 20; i++)
//  {
//    Serial.write(sdFile.read());
//  }
//  Serial.println("");
  sdFile.seekSet(id * 22);
  high = getNextULong();
  low = getNextULong();
//  Serial.print("ATL High: ");
//  Serial.println(high);
//  Serial.print("ATL Low: ");
//  Serial.println(low);
  if (amount > 0)
  {
    low += amount;
    if (low < amount)
      high++;
  }
  else
  {
    if (low < -amount)
      high++;
    low -= -amount;
  }
  sdFile.seekSet(id * 22);
  intToCurLine(high);
  sdFile.print(curLine);
  intToCurLine(low);
  sdFile.print(curLine);
  setFileAccessTime();
}

void retrieveLifetime(byte id, unsigned long *high, unsigned long *low)
{
//  curFileName[0] = 'l';
//  curFileName[1] = 'i';
//  curFileName[2] = 'f';
//  curFileName[3] = 'e';
//  curFileName[4] = 't';
//  curFileName[5] = 'i';
//  curFileName[6] = 'm';
//  curFileName[7] = 'e';
//  curFileName[8] = '.';
//  curFileName[9] = 't';
//  curFileName[10] = 'x';
//  curFileName[11] = 't';
//  curFileName[12] = '\0';
  if (!openLifetimeFile(true))
  {
    *high = 0;
    *low = 0;
    return;
  }
  sdFile.seekSet(id * 22);
  *high = getNextULong();
  *low = getNextULong();
  setFileAccessTime();
}

void retrieveData(byte id, unsigned long minute, unsigned long *data, bool *isNull)
{
  setCurFileName(id, minute / 1440);
  //if (!openFile(curFileName, true, createArchiveFile))
  if (!sdFile.open(curFileName, O_READ))
  {
    *isNull = true;
    *data = 0;
    return;
  }
  sdFile.seekSet((minute % 1440) * 12);
//  for (int i = 0; i < 200; i++)
//  {
//    Serial.write(sdFile.read());
//  }
  sdFile.seekSet((minute % 1440) * 12);
  if (sdFile.peek() == '-')
    *isNull = true;
  else
  {
    *isNull = false;
    *data = getNextULong();
  }
  if (*isNull)
    *data = 0;
  sdFile.close();
  setFileAccessTime();
}

bool archiveData(byte id, unsigned long minute, unsigned long data)
{
  unsigned long prevData;
  setCurFileName(id, minute / 1440);
//  Serial.print("CurFileName set: ");
//  Serial.println(curFileName);
  //if (!openFile(curFileName, false, createArchiveFile))
  if (!sd.exists(curFileName))
  {
    if (!sdFile.open(curFileName, O_RDWR | O_CREAT | O_TRUNC))
      return false;
    for (int i = 0; i < 1440; i++)
    {
      sdFile.println(F("----------"));
    }
  }
  else if (!sdFile.open(curFileName, O_RDWR))
    return false;
  pendingLifetimeCommitUser = id;
  sdFile.seekSet((minute % 1440) * 12);
  intToCurLine(data);
  if (sdFile.peek() == '-')
    prevData = 0;
  else
  {
    prevData = getNextULong();
    sdFile.seekSet((minute % 1440) * 12);
  }
  sdFile.println(curLine);
  sdFile.close();
  pendingLifetimeCommit += data - prevData;
  setFileAccessTime();
//  Serial.println("Archive!");
  return true;
}

//bool getStats(byte id, unsigned long day, int startMinute, int numMinutes, unsigned long *minimum, unsigned long *_total, unsigned long *maximum, unsigned long *mean, unsigned long *stdDev)
void testStats()
{
  Serial.println("Archive...");
  for (int i = 0; i < 1440; i++)
  {
    float mult = sin(((float)i)/5)*500+((float)i)/50 + 650;
    archiveData(3, i, (unsigned long)mult);
  }
  closeFile();
  Serial.println("Stats...");
  double total;
  double minimum;
  double maximum;
  double mean;
  double stdDev;
  for (int i = 0; i < 24; i++)
  {
    getHourlyStatsWH(3, 0, i, &total, &minimum, &maximum, &mean, &stdDev);
    Serial.print("Hour ");
    Serial.print(i);
    Serial.println(":");
    Serial.println("");
    Serial.print("Total (hour): \t\t");
    Serial.print(total, 5);
    Serial.println(" watt-hours");
    Serial.print("Min (minute): \t\t");
    Serial.print(minimum, 5);
    Serial.println(" watt-hours");
    Serial.print("Max (minute): \t\t");
    Serial.print(maximum, 5);
    Serial.println(" watt-hours");
    Serial.print("Mean (hour): \t\t");
    Serial.print(mean, 5);
    Serial.println(" watt-hours");
    Serial.print("Std. Dev. (hour): \t");
    Serial.print(stdDev, 5);
    Serial.println(" watt-hours");
    Serial.println("");
    Serial.println("");
    Serial.println("");
  }
}

void testLifetime()
{
  unsigned long high;
  unsigned long low;
  retrieveLifetime(2, &high, &low);
  Serial.print(low);
  Serial.print(" ");
  Serial.println(high);

  addToLifetime(2, 2000000000);
  retrieveLifetime(2, &high, &low);
  Serial.print(low);
  Serial.print(" ");
  Serial.println(high);

  addToLifetime(2, 2000000000);
  retrieveLifetime(2, &high, &low);
  Serial.print(low);
  Serial.print(" ");
  Serial.println(high);
  
  closeFile();

  addToLifetime(2, 2000000000);
  retrieveLifetime(2, &high, &low);
  Serial.print(low);
  Serial.print(" ");
  Serial.println(high);
  
  closeFile();
}

void test()
{
//  setCurFileName(2, 3300479);
//  if (!openFile(curFileName, false))
//    Serial.println("Can't open file");
//  sdFile.seekSet(0);
//  intToCurLine(3141592);
//  sdFile.println(curLine);
//  sdFile.seekSet(12);
//  intToCurLine(2718281828);
//  sdFile.println(curLine);
//  closeFile();

  archiveData(2, 2345678, 90210);
  archiveData(3, 2345678, 98122);
  archiveData(2, 2233445, 98003);
  archiveData(2, 2233443, 98023);
  archiveData(3, 2345679, 80085);

  //closeFile();

  bool isNull;
  unsigned long val;
  retrieveData(2, 2345678, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  retrieveData(3, 2345678, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  closeFile();
  retrieveData(2, 2233445, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  closeFile();
  retrieveData(2, 2233443, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  closeFile();
  retrieveData(3, 4345679, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  closeFile();
  retrieveData(3, 2345679, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  closeFile();
  retrieveData(3, 2345689, &val, &isNull);
  if (isNull)
    Serial.println("NULL!");
  else
    Serial.println(val);
  closeFile();
}

void setupArchiver()
{
  if(initArchiver(53))
    Serial.println("SD Card Initialized!");
  else
    Serial.println("SD Card Initialization failed!");
  pendingLifetimeCommitUser = 255;
}

bool initArchiver(int csPin)
{
  curLine[10] = '\0';
  if (!sd.begin(csPin, SPI_FULL_SPEED))
    return false;
  return true;
}

bool createArchiveFile(char *_fileName)
{
  Serial.print("Archive file created: ");
  Serial.println(_fileName);
  closeFile();
  if (!sdFile.open(_fileName, O_RDWR | O_CREAT | O_TRUNC))
    return false;
  for (int i = 0; i < 1440; i++)
  {
    sdFile.println(F("----------"));
  }
  sdFile.flush();
  sdFile.close();
}

bool createLifetimeFile()
{
  closeFile();
  if (!sdFile.open(lifetimeFileName, O_RDWR | O_CREAT | O_TRUNC))
    return false;
  for (int i = 0; i < 255; i++)
  {
    sdFile.println(F("00000000000000000000"));
  }
  sdFile.close();
}

void setFileAccessTime()
{
  lastFileAccessTime = millis();
}

void loopArchiver()
{
  if (sdFile.isOpen() && (millis() - lastFileAccessTime > 2000))
    closeFile();
}
*/


//void sdCardTest3()
//{
//  Serial.print("Free RAM before open file: ");
//  //Serial.println(freeRam());
//  sdFile = sd.open("test.txt", FILE_WRITE);
//  Serial.print("Free RAM after open file: ");
//  //Serial.println(freeRam());
//
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//
//  // re-open the file for reading:
//  myFile = sd->open("test.txt");
//  if (myFile) {
//    Serial.println("test.txt:");
//
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//}
//
//bool CreateArchiveFile()
//{
//  Serial.println("Initialize SdFat...");
//  //sdFatInitialize(53);
//  if (!sd->begin(53, SPI_FULL_SPEED)) {
//    sd->initErrorHalt();
//  }
//  Serial.println("Open file...");
//  if (!sdFile->open("archive.txt", O_RDWR | O_CREAT | O_AT_END))
//    return false;
//  for (int i = 0; i < 1440; i++)
//  {
//    sdFile->println(F("---------- ----------"));
//  }
//  sdFile->close();
//  Serial.println("Done.");
//}
//
//void ConfiguresdCard()
//{
//  Serial.print("Initializing sd card...");
//
//  // see if the card is present and can be initialized:
//  if (!sd->begin(53)) {
//    Serial.println("Card failed, or not present");
//    // don't do anything more:
//    return;
//  }
//  Serial.println("card initialized.");
//}
//
//void Insertsd(int dataPos, int value)
//{
//  for (int i = 0; i < 1000; i++)
//  {
//    if (i % 2 == 0)
//    {
//      file1.println(i);
//    }
//    else
//    {
//      file2.println(i);
//    }
//  }
//}
//
//bool Seeksd(int dataPos, bool toWrite)
//{
//  Serial.println("Seeksd");
//  Serial.println(file1.position());
//  if (file1.size() == 0)
//    return toWrite;
//  bool found = false;
//  bool searchDown = false;
//  bool searchedStart = false;
//  bool searchedEnd = false;
//  char curChar;
//  unsigned long pos = file1.position();
//
//  bool prevStarFound = false;
//  unsigned long prevStarPos;
//  unsigned long prevStarValue;
//  bool curStarFound = false;
//  unsigned long curStarPos;
//  unsigned long curStarValue;
//  while (!found)
//  {
//    file1.seek(pos);
//    //Serial.println(pos);
//    curChar = file1.peek();
//    //Serial.println(curChar);
//    if (curChar == '*')
//    {
//      prevStarFound = curStarFound;
//      prevStarPos = curStarPos;
//      prevStarValue = curStarValue;
//      curStarFound = true;
//      curStarPos = file1.position();
//      file1.read();
//      curStarValue = (unsigned long)file1.parseInt();
//      Serial.print("Cur star: ");
//      Serial.println(curStarValue);
//    }
//    if (curStarFound && curStarValue >= dataPos && prevStarFound && prevStarValue <= dataPos && !searchDown)
//    {
//      file1.seek(curStarPos);
//      if (toWrite)
//      {
//        file1.println("");
//        file1.seek(curStarPos);
//        return true;
//      }
//      else
//      {
//        if (curStarValue == dataPos)
//        {
//          file1.seek(curStarPos);
//          return true;
//        }
//        else if (prevStarValue == dataPos)
//        {
//          file1.seek(prevStarPos);
//          return true;
//        }
//        else
//          return false;
//      }
//    }
//    if (curStarFound && curStarValue <= dataPos && prevStarFound && prevStarValue >= dataPos && searchDown)
//    {
//      file1.seek(prevStarPos);
//      if (toWrite)
//      {
//        file1.println("");
//        file1.seek(prevStarPos);
//        return true;
//      }
//      else
//      {
//        if (curStarValue == dataPos)
//        {
//          file1.seek(curStarPos);
//          return true;
//        }
//        else if (prevStarValue == dataPos)
//        {
//          file1.seek(prevStarPos);
//          return true;
//        }
//        else
//          return false;
//      }
//    }
//    
//    if (pos == 0 && searchDown)
//    {
//      if (curStarFound && curStarValue >= dataPos)
//      {
//        if (toWrite)
//        {
//          file1.seek(0);
//          file1.println("");
//        }
//        file1.seek(0);
//        return toWrite || (curStarValue == dataPos);
//      }
//      if (searchedStart)
//        found = true;
//      else
//        searchedStart = true;
//      searchDown = false;
//    }
//    
//    if (pos == file1.size() - 1 && !searchDown)
//    {
//      if (curStarFound && curStarValue <= dataPos)
//      {
//        if (toWrite)
//        {
//          file1.seek(file1.size() - 1);
//          file1.println("");
//        }
//        file1.seek(file1.size() - 1);
//        return toWrite || (curStarValue == dataPos);
//      }
//      if (searchedEnd)
//        found = true;
//      else
//        searchedEnd = true;
//      searchDown = true;
//    }
//
//    pos = searchDown ? pos - 1 : pos + 1;
//  }
//}
//
//void Readsd()
//{
//  file1 = sd->open("test42.txt", FILE_WRITE);
//  file1.seek(0);
//  Seeksd(2, true);
//  file1.print("*2 4");
//  file1.seek(0);
//  while (file1.position() <= file1.size() - 1)
//  {
//    Serial.write(file1.read());
//  }
//  file1.close();
//}
//
//void Testsd()
//{
//  Serial.print("Free RAM before open file: ");
//  //Serial.println(freeRam());
//  file1 = sd->open("file1.txt", FILE_WRITE);
//  file2 = sd->open("file2.txt", FILE_WRITE);
//  if (file1 && file2)
//  {
//    Serial.println("File good!");
//    Serial.print("Free RAM after open file: ");
//    //Serial.println(freeRam());
//    Insertsd(0,0);
//    file1.close();
//    file2.close();
//  }
//  else
//    Serial.println("File bad.");
//  Serial.println("Done testing sd.");
//}

