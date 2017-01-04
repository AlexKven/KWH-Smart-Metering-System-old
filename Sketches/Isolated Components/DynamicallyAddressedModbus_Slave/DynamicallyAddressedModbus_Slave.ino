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

#define UNASSIGNED 247;

// data array for modbus network sharing
uint16_t Registers[4] = { 
  1, // 1 = meter, 2 = transmitter (SD card or wireless)
  0, // Newly Assigned ID (waiting for this from master)
  0, // Previously Assigned ID (if deactivated)
  0
  };

uint8_t SlaveID = 247;

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
  Serial.begin(9600);
  Serial.println("Initialized unassigned");
  slave.begin(9600); // baud-rate at 19200
}

void loop()
{
  slave.poll(Registers, 4);
  if (SlaveID == 247) //Slave is unassigned
  {
    if (Registers[1] > 0) //Slave was just assigned
    {
      SlaveID = Registers[1];
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
