/**
 *  Modbus master example 1:
 *  The purpose of this example is to query an array of data
 *  from an external Modbus slave device. 
 *  The link media can be USB or RS232.
 *
 *  Recommended Modbus slave: 
 *  diagslave http://www.modbusdriver.com/diagslave.html
 *
 *  In a Linux box, run 
 *  "./diagslave /dev/ttyUSB0 -b 19200 -d 8 -s 1 -p none -m rtu -a 1"
 *   This is:
 *    serial port /dev/ttyUSB0 at 19200 baud 8N1
 *    RTU mode and address @1
 */

#include <ModbusRtu.h>

#define UNASSIGNED 247;
const int MAX_DEVICES = 256;

// data array for modbus network sharing
uint16_t au16data[16];
uint16_t writeval;
uint8_t u8state;

uint8_t* DeviceDirectory;

SoftwareSerial serial(10, 11);

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0,1,4); // this is master and RS-232 or USB-FTDI

/**
 * This is an structe which contains a query to an slave device
 */
modbus_t telegram;

unsigned long u32wait;
int numSlaves = 0;

void setup()
{
  DeviceDirectory = new uint8_t[MAX_DEVICES * 10];
  
  Serial.begin(9600);
  master.begin( 9600 ); // baud-rate at 19200
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 2000;
  u8state = 0; 
}

int AddToDeviceDirectory(uint8_t* devName8, uint8_t devType, slaveID)
{
  int row = 0;
  while (DeviceDirectory[row * 10 + 8] != 0)
    row++;
  int ind = 10 * row;
  for (int i = 0; i < 8; i++)
    DeviceDirectory[ind + i] = devName8[i];
  DeviceDirectory[ind + 8] = devType;
  DeviceDirectory[ind + 9] = slaveID;
  return row;
}

void clearDeviceDirectory(int row)
{
  int ind = 10 * row;
  for (int i = 0; i < 9; i++)
    DeviceDirectory[ind + i] = 0;
  if (row == MAX_DEVICES - 1 || DeviceDirectory[ind + 18] == 0)
    DeviceDirectory[ind + 9] = 0;
  else
    DeviceDirectory[ind + 9] = 1;
  if (row > 0 && DeviceDirectory[ind - 2] == 0)
    DeviceDirectory[ind - 1] = 0;
}

void loop() {
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    telegram.u8id = UNASSIGNED; // slave address
    telegram.u8fct = 3; // function code (this one is registers read)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 4; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    u8state++;
    break;
  case 2:
    master.poll(); // check incoming messages
      Serial.println(master.getState());
    if (master.getState() == COM_IDLE)
    {
      if (au16data[0] == 0)
      {
        if (master.getTimeOutState())
        {
          master.setTimeOut(2000);
          u8state = 0;
        }
      }
      else
      {
        u8state = 3;
        u32wait = millis() + 2000;
        Serial.println("Found unassigned slave.");
        Serial.print("Slave type: ");
        Serial.println(au16data[0]);
        Serial.print("Previous ID: ");
        Serial.println(au16data[2]);
        au16data[0] = 0;
      }
    }
    delay(500);
    break;
  case 3:
    writeval = numSlaves + 1;
    telegram.u8id = UNASSIGNED; // slave address
    telegram.u8fct = 16; // function code (this one is registers write)
    telegram.u16RegAdd = 1; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    telegram.au16reg = &writeval; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    u8state++;
    break;
  case 4:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      numSlaves++;
      Serial.print("Successfully assigned slave # ");
      Serial.println(numSlaves);
    }
    break;
  }
  delay(1);
}

