#include <C:\GitHub\KWH-Smart-Metering-System\2017\Libraries\DeviceDirectory.h>
#include <C:\GitHub\KWH-Smart-Metering-System\2017\Libraries\ModbusCommunicator.cpp>

void setup() {
  // put your setup code here, to run once:
  DeviceDirectory<int8_t, int8_t, 8, 20> dd;
  ModbusCommunicator mc;
  mc.loop(50);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
