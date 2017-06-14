#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include "C:\GitHub\KWH-Smart-Metering-System\Sketches\Isolated Components\DynamicallyAddressedModbus_Master\DeviceDirectory.ino"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		uint8_t* to_arstring(string *str)
		{
			int len = str->length();
			uint8_t* result = new uint8_t[len + 1];
			for (int i = 0; i < len; i++)
				result[i] = str->at(i);
			result[len] = '\0';
			return result;
		}

		uint8_t* to_arstring(const char* str)
		{
			int len = 0;
			while (str[len] != '\0')
				len++;
			uint8_t* result = new uint8_t[len + 1];
			for (int i = 0; i < len; i++)
				result[i] = str[i];
			result[len] = '\0';
			return result;
		}

		TEST_METHOD(DeviceDirectory_BasicTests)
		{
			DeviceDirectory_ directory;
			directory.InitializeDeviceDirectory();
			int s1 = directory.AddToDeviceDirectory(to_arstring("Aleah703"), 1, 1);
			Assert::IsTrue(directory.FindFreeRow() == 1);
			int s2 = directory.AddToDeviceDirectory(to_arstring("Aleah704"), 1, 2);
			Assert::IsTrue(directory.FindFreeRow() == 2);
			uint8_t *name = to_arstring("Aleah703");
			directory.DeleteDevicesForSlaveNotInList(&name, 1, 1);
			Assert::IsTrue(directory.FindFreeRow() == 2);
		}
	};
}