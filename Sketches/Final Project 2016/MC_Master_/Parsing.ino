#define MAX_GPRS_LINE_SIZE 128
char p_curGPRSLine[MAX_GPRS_LINE_SIZE];
int p_curGPRSLinePos = 0;
char msgId[3];
int numLinesIgnore = 0;
bool SMSWaiting = false;

void parseInput(char input)
{
  if (input == '\r' || input == '\n')
  {
    if (p_curGPRSLinePos != 0)
    {
      p_curGPRSLine[p_curGPRSLinePos] = '\0';
      //Serial.println(p_curGPRSLine);
      parseLine();
      p_curGPRSLinePos = 0;
    }
  }
  else
  {
    p_curGPRSLine[p_curGPRSLinePos] = input;
    if (p_curGPRSLinePos < MAX_GPRS_LINE_SIZE - 1)
      p_curGPRSLinePos++;
  }
}

inline void parseLine()
{
  unsigned long time;
  if (numLinesIgnore > 0)
  {
    numLinesIgnore--;
  }
  else
  {
    if (SMSWaiting)
    {
      Serial.println("SMS!");
      Serial.println(p_curGPRSLine);
      int id = 0;
      int i = 0;
      while (p_curGPRSLine[i] >= '0' && p_curGPRSLine[i] <= '9')
      {
       id *= 10;
       id += (p_curGPRSLine[i] - '0');
       i++;
      }
      Serial.println(id);
      if (p_curGPRSLine[i + 1] == 'o' && p_curGPRSLine[i + 2] == 'f' && p_curGPRSLine[i + 3] == 'f')
      {
        Serial.print("Turn ");
        Serial.print(id);
        Serial.println(" off.");
        secretary->requestPowerSet(id, false);
      }
      else if (p_curGPRSLine[i + 1] == 'o' && p_curGPRSLine[i + 2] == 'n')
      {
        Serial.print("Turn ");
        Serial.print(id);
        Serial.println(" on.");
        secretary->requestPowerSet(id, true);
      }
      deleteMessage(msgId);
      SMSWaiting = false;
    }
    else if (p_curGPRSLine[0] == '+' && p_curGPRSLine[1] == 'C' && p_curGPRSLine[2] == 'C' && p_curGPRSLine[3] == 'L' && p_curGPRSLine[4] == 'K' && p_curGPRSLine[5] == ':')
    {
      time = parseMinuteFromFormat(p_curGPRSLine + 8);
      setCurrentTime(time);
      secretary->announceTime(time);
    }//+CMTI: "SM",
    else if (p_curGPRSLine[0] == '+' && p_curGPRSLine[1] == 'C' && p_curGPRSLine[2] == 'M' && p_curGPRSLine[3] == 'T' && p_curGPRSLine[4] == 'I' && p_curGPRSLine[5] == ':' && p_curGPRSLine[8] == 'S' && p_curGPRSLine[9] == 'M')
    {
      bool done = false;
      for (int i = 0; i < 3; i++)
      {
       if (1 == 2)
        msgId[i] = '\0';
       else if (!done)
       {
         msgId[i] = p_curGPRSLine[12 + i];
         if (p_curGPRSLine[12 + i] == '\0')
           done = true;
       } 
      }
      requestMessage(msgId);
      SMSWaiting = true;
      numLinesIgnore = 2;
    }
    else
      Serial.println(p_curGPRSLine);
  }
}

unsigned long parseMinuteFromFormat(char *format)
{
  int year = (format[0] - '0') * 10 + (format[1] - '0') - 16;
  int month = (format[3] - '0') * 10 + (format[4] - '0');
  int day = (format[6] - '0') * 10 + (format[7] - '0');
  int hour = (format[9] - '0') * 10 + (format[10] - '0');
  int minute = (format[12] - '0') * 10 + (format[13] - '0');
  unsigned long result = 0;
  result += year * 365 + ((year + 3) / 4);
  if (month > 1)
    result += 31;
  if (month > 2)
    result += (year % 4 == 0 ? 29 : 28);
  if (month > 3)
    result += 31;
  if (month > 4)
    result += 30;
  if (month > 5)
    result += 31;
  if (month > 6)
    result += 30;
  if (month > 7)
    result += 31;
  if (month > 8)
    result += 31;
  if (month > 9)
    result += 30;
  if (month > 10)
    result += 31;
  if (month > 11)
    result += 30;
  result += day - 1;
  result *= 1440;
  result += hour * 60;
  result += minute;
  return result;
  //Serial.println(result);
}

//#define DATA_LOG            "datalog.txt"
//#define MAX_CUSTOMERS       255
//#define CC_LENGTH           1
//#define AC_LENGTH           3
//#define PN_LENGTH           7
//#define TOTAL_PN_LENGTH     (CC_LENGTH + AC_LENGTH + PN_LENGTH)
//#define DELIMITER           ','
//#define METER_NUM_SIZE      3
//#define USAGE_SIZE          5
//#define MAX_CHAR_SIZE       20
//
////SoftwareSerial gprsSerial(10, 8);
//#define gprsSerial Serial1
//String p_nextMessage = "";
//long p_tme = 0;
//
//#define MAX_AT_RESPONSE_SIZE 128
//#define _NONE -1
//#define _PARSING 0
//#define AT_UNKNOWN 1
//#define SMS_READ 2
//#define SMS_DETAIL 3
//#define DATE_TIME 3
//char p_atResponse[MAX_AT_RESPONSE_SIZE];
//int P_curAtResponseInd = 0;
//int p_curCommand = _NONE;
//bool p_inQuotes = false;
////int p_curParam = 0;
//char p_ignoreChar = '\0';
//bool p_escaped = false;
//bool p_paramMode = false;
//bool p_contentMode = false;
//
//void ParseInput(char input)
//{
//  bool concatResp = false;
//  bool paramMode = false;
//  if (input == p_ignoreChar)
//  {
//    return;
//  }
//  p_ignoreChar = '\0';
//  if (p_escaped)
//  {
//    concatResp = true;
//  }
//  else
//  {
//    switch (p_curCommand)
//    {
//      case _NONE:
//        if (input == '+')
//        {
//          p_curCommand = _PARSING;
//        }
//      break;
//      case _PARSING:
//        if (input == ':')
//        {
//          addToAtResponse('\0');
//          //Serial.print(" AT Resp: ");
//          Serial.println(p_atResponse);
//          if (strcmp(p_atResponse, "CMTI") == 0)
//            p_curCommand = SMS_READ;
//          else if (strcmp(p_atResponse, "CCLK") == 0)
//            p_curCommand = DATE_TIME;
//          else
//            resetParsingGlobals(_NONE);
//          p_ignoreChar = ' ';
//        }
//        else
//        {
//          //Serial.println("else");
//          concatResp = true;
//        }
//      break;
//      case AT_UNKNOWN:
//        if (input == '\r' || input == '\n')
//            resetParsingGlobals(_NONE);
//      break;
//      case SMS_READ:
//        //Serial.print('r'); \\dbg
//  //      if (curChar == '\\')
//  //        ignorChar = '"';
//          p_paramMode = true;
//          //Serial.println("SMS_READ");
////          Serial.print("Input: ");
////          Serial.println(input);
////          Serial.print("AT Resp: ");
////          Serial.println(p_atResponse);
//          if (input == '\r')
//          {
//            requestMessage(findParameter(p_atResponse, 2, MAX_AT_RESPONSE_SIZE));
//            resetParsingGlobals(_NONE);
//          }
//      break;
//      case DATE_TIME:
//        //Serial.print('r'); \\dbg
//  //      if (curChar == '\\')
//  //        ignorChar = '"';
//          p_paramMode = true;
//          Serial.println("DATE_TIME");
////          Serial.print("Input: ");
////          Serial.println(input);
////          Serial.print("AT Resp: ");
////          Serial.println(p_atResponse);
//          if (input == '\r')
//          {
//            Serial.println(findParameter(p_atResponse, 1, MAX_AT_RESPONSE_SIZE));
//            resetParsingGlobals(_NONE);
//          }
//      break;
//    }
//  }
//  p_escaped = false;
//  if (p_paramMode)
//  {
//      if (input == '"')
//        p_inQuotes = !p_inQuotes;
//      else
//      {
//      if (p_inQuotes)
//          concatResp = true;
//        else
//        {
//          if (input == ',')
//          {
//            //p_curParam++;
//            addToAtResponse('\0');
//          }
//          else if (input == '\r')
//          {
//            p_curCommand = _NONE;
//            addToAtResponse('\0');
//          }
//          else
//            concatResp = true;
//        }
//      }
//  }
//  if (concatResp)
//  {
//    addToAtResponse(input);
////    bool done = false
////    for (int i = p_paramStartPositions[p_numParameters - 1]; i < MAX_AT_RESPONSE_SIZE - 1; i++)
////    {
////      if (!done && p_atResponse[i] == '\0')
////      {
////        p_atResponse[i] = input;
////        p_atResponse[i + 1] = '\0';
////        done = true;
////      }
////    }
////    P_curAtResponseInd++;
//  }
//}
//
//void resetParsingGlobals(int command)
//{
//  P_curAtResponseInd = 0;
//  p_curCommand = command;
//  p_inQuotes = false;
//  p_ignoreChar = '\0';
//  p_escaped = false;
//  p_paramMode = false;
//  p_contentMode = false;
//  for (int i = 0; i < MAX_AT_RESPONSE_SIZE; i++)
//    p_atResponse[i] = '\0';
//}
//
//void addToAtResponse(char chr)
//{
//  if (P_curAtResponseInd < MAX_AT_RESPONSE_SIZE)
//  {
//    p_atResponse[P_curAtResponseInd] = chr;
//  }
//  P_curAtResponseInd++;
//}
//
//char *findParameter(char *str, int index, int len)
//{
//  int ind = 0;
//  int nullCount = 0;
//  while (nullCount != index && ind < len)
//  {
//    if (str[ind] == '\0')
//      nullCount++;
//    ind++;
//  }
//  return str + ind;
//}

