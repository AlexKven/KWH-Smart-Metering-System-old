#include "Modbus-Master-Slave-for-Arduino-master\ModbusRtu.h"
#include "Communicator.h"

class ModbusCommunicator : Communicator<int8_t, int8_t, unsigned long, 8>
{
private:

	// data array for modbus network sharing
	uint16_t au16data[64];
	uint16_t writeval;
	uint8_t u8state;

	SoftwareSerial serial(10, 11);

	/**
	*  Modbus object declaration
	*  u8id : node id = 0 for master, = 1..247 for slave
	*  u8serno : serial port (use 0 for Serial)
	*  u8txenpin : 0 for RS-232 and USB-FTDI
	*               or any pin number > 1 for RS-485
	*/
	Modbus modbus(0, 1, 4); // this is master and RS-232 or USB-FTDI

							/**
							* This is an structe which contains a query to an slave device
							*/
	modbus_t telegram;

	void initializeInternal();

public:
	void loop(unsigned long millis);
};
