const int NUM_METERS = 3;
const int METER_POLL_INTERVAL_MINS = 1;

unsigned long currentUsage[NUM_METERS];
bool currentIsNull[NUM_METERS];

void setup() 
{
  Serial.begin(9600);
  Serial.println("Master!");
  setupTime();
  setupMeter();
  setupCommunication();
  for (int i = 0; i < NUM_METERS; i++)
  {
   currentIsNull[i] = true; 
  }
  //<NOSD> setupArchiver();
  setupGPRS();
  setupTasks();
  delay(1000);
//  unsigned long d;
//  bool nul;
//  retrieveData(1, 215387, &d, &nul);
//  Serial.print("215389: ");
//  if (nul)
//    Serial.println("Null");
//  else
//    Serial.println(d);
//  archiveData(0, 214000, 500);
//  archiveData(1, 214000, 550);
//  archiveData(0, 214001, 600);
//  archiveData(1, 214001, 650);
//  archiveData(0, 214002, 700);
//  archiveData(1, 214002, 750);
//  archiveData(0, 214003, 800);
//  archiveData(1, 214003, 850);
}

void loop() 
{
  String str;
  char chr;
  loopTime();
  loopMeter();
  loopCommunication();
  //<NOSD>loopArchiver();
  loopGPRS();
  loopTasks();
  if (Serial.available())
  {
    chr = Serial.read();
    switch (chr)
    {
      case 'c':
        RequestTime();
      break;
      case 't':
        testGprs();
      break;
      case 'm':
        SendSMSMessage("Hello!");
      break;
      case 'd':
        delay(20);
        chr = Serial.read();
        str = "";
        while (chr >= '0' && chr <= '9')
        {
          str.concat(chr);
          delay(20);
          chr = Serial.read();
        }
        deleteMessage(str.c_str());
      break;
      case 'r':
        delay(20);
        chr = Serial.read();
        str = "";
        while (chr >= '0' && chr <= '9')
        {
          str.concat(chr);
          delay(20);
          chr = Serial.read();
        }
        requestMessage(str.c_str());
      break;
    }
  }
}

