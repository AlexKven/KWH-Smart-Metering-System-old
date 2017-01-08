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
#include "RegisterData.cpp"

#define UNASSIGNED 247;
const int MAX_DEVICES_PER_SLAVE = 5;
const int MAX_RESPONSE_SIZE = 120;
const int MAX_TRANSMISSION_SIZE = 120;

enum
{
  WAIT = 0,
  CHECK_NEW_DEVICES = 1,
  CHECK_NEW_DEVICES_RESPONSE = 2,
  RESPOND_NEW_DEVICE = 3,
  RESPOND_NEW_DEVICE_RESPONSE = 4,
};

// data array for modbus network sharing
RegisterData *Transmission;
RegisterData *Response;
uint8_t State;

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

void setup()
{
  Transmission = new RegisterData(MAX_TRANSMISSION_SIZE);
  Response = new RegisterData(MAX_RESPONSE_SIZE);
  InitializeDeviceDirectory();
  
  Serial.begin(9600);
  master.begin( 19200 ); // baud-rate at 19200
  master.setTimeOut( 50 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 50;
  State = CHECK_NEW_DEVICES; 
}

void loop() {
  switch( State ) {
  case WAIT: 
    if (millis() > u32wait) State++; // wait state
    break;
  case CHECK_NEW_DEVICES: 
    telegram.u8id = UNASSIGNED; // slave address
    telegram.u8fct = 3; // function code (this one is registers read)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = MAX_DEVICES_PER_SLAVE * 5; // number of elements (coils or registers) to read
    telegram.au16reg = Response->getArray(); // pointer to a memory array in the Arduino

    Response->set(0, 0);
    master.query( telegram ); // send query (only once)
    State++;
    break;
  case CHECK_NEW_DEVICES_RESPONSE:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE)
    {
      if (Response->get(0) == 0)
      {
        if (master.getTimeOutState())
        {
          //Serial.println("Timeout!");
          master.setTimeOut(50);
          State = WAIT;
        }
      }
      else
      {
        State = RESPOND_NEW_DEVICE;
        u32wait = millis() + 50;
        uint8_t newID;
        uint8_t dummy;
        bool found = FindDeviceDetails(Response->getArray<uint8_t>() + 1, &dummy, &newID);
        if (!found)
          newID = FindFreeSlaveID();
        int numDevices = 0;
        while (numDevices < MAX_DEVICES_PER_SLAVE && Response->get<uint8_t>(numDevices * 10) != 0)
          numDevices++;
        Transmission->set<uint8_t>(0, 0);
        Transmission->set<uint8_t>(1, newID);
        Serial.print("Found unassigned slave with ");
        Serial.print(numDevices);
        Serial.println(" devices.");
        if (found)
        {
          Serial.print("This device is already associated with SlaveID #");
          Serial.println(newID);
        }
        for (int i = 0; i < numDevices; i++)
        {
          Serial.print("Device type: ");
          Serial.println(Response->get<uint8_t>(i * 10));
          Serial.print("Device name: ");
          for (int j = 0; j < 8; j++)
            Serial.write(Response->get<char>(i * 10 + 1 + j));
          Serial.println("");
          if (!FindDeviceDetails(Response->getArray<uint8_t>() + i * 10 + 1, &dummy, &dummy))
            AddToDeviceDirectory(Response->getArray<uint8_t>() + i * 10 + 1, Response->get<uint8_t>(i * 10), newID);
        }
      }
    }
    break;
  case RESPOND_NEW_DEVICE:
    Serial.println("Responding...");
    telegram.u8id = UNASSIGNED; // slave address
    telegram.u8fct = 16; // function code (this one is registers write)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    telegram.au16reg = Transmission->getArray(); // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    State++;
    break;
  case 4:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      State = 0;
      Serial.print("Successfully assigned slave # ");
      Serial.println(Transmission->get<uint8_t>(1));
    }
    break;
  }
  delay(1);
}

