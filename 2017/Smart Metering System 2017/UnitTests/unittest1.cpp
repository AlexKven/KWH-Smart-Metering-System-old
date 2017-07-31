#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\..\Libraries\DeviceDirectory.h"
#include <string>
#include "HelperFunctions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef DeviceDirectory<short, int, 32767, 4> BigDeviceDirectory;
typedef DeviceDirectory<char, short, 5, 2> SmallDeviceDirectory;
typedef short BigDDName[4];
typedef char SmallDDName[2];

namespace UnitTests
{		
	TEST_CLASS(DeviceDirectoryTests)
	{
	public:

		template<typename T, int S>
		void copyArray(T destination[S], T source[S])
		{
			for (int i = 0; i < S; i++)
			{
				destination[i] = source[i];
			}
		}
		
		TEST_METHOD(DeviceDirectory_Functionality)
		{
			BigDeviceDirectory bigDD;
			SmallDeviceDirectory smallDD;

			//Test case 1: Add device, retrieve values
			BigDDName bigName;
			for (int i = 0; i < 4; i++)
				bigName[i] = 0;
			bigName[2] = 1;
			expected_value<bool>(bigDD.AddDevice(1, 3, bigName), true, L"1.-.0");
			bigName[2] = 2;
			expected_value<bool>(bigDD.AddDevice(1, 2, bigName), true, L"1.-.1");
			bigName[2] = 3;
			expected_value<bool>(bigDD.AddDevice(5, 6, bigName), true, L"1.-.2");
			short type;
			short slaveID;

			//Test 1.0: Retrieve device "0010"
			bigName[2] = 1;
			expected_value<short>(bigDD.searchNext(0, &type, &slaveID, &bigName, false, false, true), 0, L"1.0.0");
			expected_value<short>(type, 1, L"1.0.1");
			expected_value<short>(slaveID, 3, L"1.0.2");

			//Test 1.1: Retrieve device "0030"
			bigName[2] = 3;
			expected_value<short>(bigDD.searchNext(0, &type, &slaveID, &bigName, false, false, true), 2, L"1.1.0");
			expected_value<short>(type, 5, L"1.1.1");
			expected_value<short>(slaveID, 6, L"1.1.2");

			//Test 1.2: Retrieve device "0020"
			bigName[2] = 2;
			expected_value<bool>(bigDD.searchNext(0, &type, &slaveID, &bigName, false, false, true), 1, L"1.2.0");
			expected_value<short>(type, 1, L"1.2.1");
			expected_value<short>(slaveID, 2, L"1.2.2");

			//Test 1.3: Retrieve devices from slave ID 2
			bigName[2] = 4;
			expected_value<bool>(bigDD.AddDevice(3, 2, bigName), true, L"1.3.0");
			bigName[2] = 5;
			expected_value<bool>(bigDD.AddDevice(3, 3, bigName), true, L"1.3.1");
			bigName[2] = 6;
			expected_value<bool>(bigDD.AddDevice(8, 2, bigName), true, L"1.3.2");
			bigName[2] = 7;
			expected_value<bool>(bigDD.AddDevice(3, 6, bigName), true, L"1.3.3");

			slaveID = 2;
			expected_value<short>(bigDD.searchNext(0, &type, &slaveID, &bigName, false, true, false), 1, L"1.3.4");
			expected_value<char>(type, 1, L"1.3.5");
			expected_value<char>(bigName[2], 2, L"1.3.6");

			expected_value<short>(bigDD.searchNext(2, &type, &slaveID, &bigName, false, true, false), 3, L"1.3.7");
			expected_value<char>(type, 3, L"1.3.8");
			expected_value<char>(bigName[2], 4, L"1.3.9");

			expected_value<short>(bigDD.searchNext(4, &type, &slaveID, &bigName, false, true, false), 5, L"1.3.10");
			expected_value<char>(type, 8, L"1.3.11");
			expected_value<char>(bigName[2], 6, L"1.3.12");

			expected_value<short>(bigDD.searchNext(6, &type, &slaveID, &bigName, false, true, false), 32767, L"1.3.13");

			//Test case 2: overfill directory
			SmallDDName smallName;
			smallName[1] = 0;
			smallName[0] = 1;
			expected_value<bool>(smallDD.AddDevice(1, 9, smallName), true, L"2.-.0");
			smallName[0] = 3;
			expected_value<bool>(smallDD.AddDevice(2, 8, smallName), true, L"2.-.1");
			smallName[0] = 5;
			expected_value<bool>(smallDD.AddDevice(3, 7, smallName), true, L"2.-.2");
			smallName[0] = 7;
			expected_value<bool>(smallDD.AddDevice(4, 6, smallName), true, L"2.-.3");

			//Test 2.0: fill directory
			smallName[0] = 9;
			expected_value<bool>(smallDD.AddDevice(5, 5, smallName), true, L"2.0.0");

			//Test 2.1: overfill directory
			smallName[0] = 11;
			expected_value<bool>(smallDD.AddDevice(6, 4, smallName), false, L"2.1.0");

			char small_type;
			char small_slaveID;

			//Test 2.2: retrieve first element
			smallName[0] = 1;
			expected_value<char>(smallDD.searchNext(0, &small_type, &small_slaveID, &smallName, false, false, true), 0, L"2.2.0");
			expected_value<char>(small_type, 1, L"2.2.1");
			expected_value<char>(small_slaveID, 9, L"2.2.2");

			//Test 2.3: retrieve middle element
			smallName[0] = 5;
			expected_value<char>(smallDD.searchNext(0, &small_type, &small_slaveID, &smallName, false, false, true), 2, L"2.3.0");
			expected_value<char>(small_type, 3, L"2.3.1");
			expected_value<char>(small_slaveID, 7, L"2.3.2");

			//Test 2.4: retrieve last element
			smallName[0] = 9;
			expected_value<char>(smallDD.searchNext(0, &small_type, &small_slaveID, &smallName, false, false, true), 4, L"2.4.0");
			expected_value<char>(small_type, 5, L"2.4.1");
			expected_value<char>(small_slaveID, 5, L"2.4.2");

			//Test 2.5: retrieve overfilled element
			smallName[0] = 11;
			expected_value<char>(smallDD.searchNext(0, &small_type, &small_slaveID, &smallName, false, false, true), 5, L"2.5.0");

			//Test case 3: removing and re-adding
			//Test 3.0 remove 2nd element
			smallName[0] = 3;
			expected_value<bool>(smallDD.RemoveDevice(smallDD.searchNext(0, NULL, NULL, &smallName, false, false, true)), true, L"3.0.0");
			expected_value<char>(smallDD.data[1].deviceType, 0, L"3.0.1");
			expected_value<char>(smallDD.data[1].deviceSlaveID, 1, L"3.0.2");

			//Test 3.1 remove first element
			smallName[0] = 1;
			expected_value<bool>(smallDD.RemoveDevice(smallDD.searchNext(0, NULL, NULL, &smallName, false, false, true)), true, L"3.1.0");
			expected_value<char>(smallDD.data[0].deviceType, 0, L"3.1.1");
			expected_value<char>(smallDD.data[0].deviceSlaveID, 1, L"3.1.2");

			//Test 3.2 remove last element
			smallName[0] = 9;
			expected_value<bool>(smallDD.RemoveDevice(smallDD.searchNext(0, NULL, NULL, &smallName, false, false, true)), true, L"3.2.0");
			expected_value<char>(smallDD.data[4].deviceType, 0, L"3.2.1");
			expected_value<char>(smallDD.data[4].deviceSlaveID, 0, L"3.2.2");
			expected_value<char>(smallDD.data[0].deviceSlaveID, 1, L"3.2.3");

			//Test 3.3 remove 4th element
			smallName[0] = 7;
			expected_value<bool>(smallDD.RemoveDevice(smallDD.searchNext(0, NULL, NULL, &smallName, false, false, true)), true, L"3.3.0");
			expected_value<char>(smallDD.data[3].deviceType, 0, L"3.3.1");
			expected_value<char>(smallDD.data[3].deviceSlaveID, 0, L"3.3.2");
			expected_value<char>(smallDD.data[0].deviceSlaveID, 1, L"3.3.3");

			//Test 3.4 remove 3rd element
			smallName[0] = 5;
			expected_value<bool>(smallDD.RemoveDevice(smallDD.searchNext(0, NULL, NULL, &smallName, false, false, true)), true, L"3.4.0");
			expected_value<char>(smallDD.data[2].deviceType, 0, L"3.4.1");
			expected_value<char>(smallDD.data[2].deviceSlaveID, 0, L"3.4.2");
			expected_value<char>(smallDD.data[0].deviceSlaveID, 0, L"3.4.3");

			//Test 3.4 remove non-existent element
			smallName[0] = 11;
			expected_value<bool>(smallDD.RemoveDevice(smallDD.searchNext(0, NULL, NULL, &smallName, false, false, true)), false, L"3.5.0");
		}

		TEST_METHOD(DeviceDirectory_TimeComplexity)
		{
			BigDeviceDirectory dd;
			BigDDName name;
			name[3] = 1;
			dd.data[0].deviceType = 3;
			dd.data[0].deviceSlaveID = 2;
			dd.copyName(dd.data[0].deviceName, name);
			dd.data[1].deviceSlaveID = 1;
			dd.data[2].deviceSlaveID = 1;
			dd.data[3].deviceSlaveID = 1;
			name[3] = 2;
			dd.data[4].deviceType = 1;
			dd.data[4].deviceSlaveID = 1;
			dd.copyName(dd.data[4].deviceName, name);
			for (int i = 5; i < 10000; i++)
			{
				dd.data[i].deviceSlaveID = 1;
			}
			name[3] = 3;
			dd.data[10000].deviceType = 7;
			dd.data[10000].deviceSlaveID = 1;
			dd.copyName(dd.data[10000].deviceName, name);
			for (int i = 10001; i < 20000; i++)
			{
				dd.data[i].deviceSlaveID = 1;
			}

			//Test case 1: getting device details
			dd.testCounter = 0;
			name[3] = 1;
			dd.searchNext(0, NULL, NULL, &name, false, false, true);
			expected_value<int>(dd.testCounter, 0, L"1.0");
			dd.testCounter = 0;
			name[3] = 2;
			dd.searchNext(0, NULL, NULL, &name, false, false, true);
			expected_value<int>(dd.testCounter, 4, L"1.1");
			dd.testCounter = 0;
			name[3] = 3;
			dd.searchNext(0, NULL, NULL, &name, false, false, true);
			expected_value<int>(dd.testCounter, 10000, L"1.2");
			dd.testCounter = 0;
			name[3] = 4;
			dd.searchNext(0, NULL, NULL, &name, false, false, true);
			expected_value<int>(dd.testCounter, 20000, L"1.3");
		}
	};
}