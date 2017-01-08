/**
 *  Modbus slave example 1:
 *  The purpose of this example is to link a data array
 *  from the Arduino to an external device.
 *
 *  Recommended Modbus Master: QModbus
 *  http://qmodbus.sourceforge.net/
 */

#include <ModbusRtu.h>
#include <SoftwareSerial.h>
#include "RegisterData.cpp"

#define UNASSIGNED 247;

// data array for modbus network sharing
RegisterData *Registers;
// = { 
//  1, // 1 = meter, 2 = transmitter (SD card or wireless)
//  0, // Newly Assigned ID (waiting for this from master)
//  0, // Previously Assigned ID (if deactivated)
//  0
//  };

uint8_t SlaveID = 247;

char* Dev1Name = const_cast<char*>("Cha_Mtr0");
char* Dev2Name = const_cast<char*>("Cha_Arc0");
char Dev1Type = 1;
char Dev2Type = 2;

SoftwareSerial serial(10, 11);

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus slave(247,1,4); // this is slave @1 and RS-232 or USB-FTDI

void setup()
{
  Registers = new RegisterData(25);
  Serial.begin(9600);
  Serial.println("Initialized unassigned");
  slave.begin(19200); // baud-rate at 19200
  Registers->set<uint8_t>(0, Dev1Type);
  Registers->set<uint8_t>(10, Dev2Type);
  Registers->set<uint8_t>(30, 0);
  Registers->setString(1, Dev1Name, 8);
  Registers->setString(11, Dev2Name, 8);
  for (int i = 0; i < 20; i++)
  {
    Serial.println(Registers->get<uint8_t>(i));
  }
}

void loop()
{
  uint8_t *bytes = Registers->getArray<uint8_t>();
  slave.poll(Registers->getArray(), Registers->getLength());
  if (SlaveID == 247) //Slave is unassigned
  {
    if (Registers->get<uint8_t>(0) == 0) //Slave was just assigned
    {
      SlaveID = Registers->get<uint8_t>(1);
      slave.setID(SlaveID);
      Serial.print("Successfully assigned slave # ");
      Serial.println(SlaveID);
    }
  }
  else
  {

  }
  delay(1);
}
