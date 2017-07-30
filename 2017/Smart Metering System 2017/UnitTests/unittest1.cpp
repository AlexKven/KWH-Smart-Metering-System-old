#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\..\Libraries\DeviceDirectory.h"
#include <string>
#include "HelperFunctions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef DeviceDirectory<short, int, 10, 4> BigDeviceDirectory;
typedef DeviceDirectory<char, short, 5, 2> SmallDeviceDirectory;
typedef short BigDDName[4];
typedef char SmallDDName[2];

namespace UnitTests
{		
	TEST_CLASS(UnitTest1)
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
		
		TEST_METHOD(DeviceDirectory)
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

			bigName[2] = 1;
			//Test 1.0: Retrieve device "0010"
			expected_value<bool>(bigDD.GetDeviceDetails(bigName, &type, &slaveID), true, L"1.0.0");
			expected_value<short>(type, 1, L"1.0.1");
			expected_value<short>(slaveID, 3, L"1.0.2");

			bigName[2] = 3;
			//Test 1.1: Retrieve device "0030"
			expected_value<bool>(bigDD.GetDeviceDetails(bigName, &type, &slaveID), true, L"1.1.0");
			expected_value<short>(type, 5, L"1.1.1");
			expected_value<short>(slaveID, 6, L"1.1.2");

			bigName[2] = 2;
			//Test 1.2: Retrieve device "0020"
			expected_value<bool>(bigDD.GetDeviceDetails(bigName, &type, &slaveID), true, L"1.2.0");
			expected_value<short>(type, 1, L"1.2.1");
			expected_value<short>(slaveID, 2, L"1.2.2");

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
			expected_value<bool>(smallDD.GetDeviceDetails(smallName, &small_type, &small_slaveID), true, L"2.2.0");
			expected_value<short>(small_type, 1, L"2.2.1");
			expected_value<short>(small_slaveID, 9, L"2.2.2");

			//Test 2.3: retrieve middle element
			smallName[0] = 5;
			expected_value<bool>(smallDD.GetDeviceDetails(smallName, &small_type, &small_slaveID), true, L"2.3.0");
			expected_value<short>(small_type, 3, L"2.3.1");
			expected_value<short>(small_slaveID, 7, L"2.3.2");

			//Test 2.4: retrieve last element
			smallName[0] = 9;
			expected_value<bool>(smallDD.GetDeviceDetails(smallName, &small_type, &small_slaveID), true, L"2.4.0");
			expected_value<short>(small_type, 5, L"2.4.1");
			expected_value<short>(small_slaveID, 5, L"2.4.2");

			//Test 2.5: retrieve overfilled element
			smallName[0] = 11;
			expected_value<bool>(smallDD.GetDeviceDetails(smallName, &small_type, &small_slaveID), false, L"2.5.0");

			//Test case 3: removing and re-adding
			//Test 3.0 remove 2nd element
			smallName[0] = 3;
			expected_value<bool>(smallDD.RemoveDevice(smallName, NULL, NULL), true, L"3.0.0");
			expected_value<bool>(smallDD.data[1].deviceType, 0, L"3.0.1");
			expected_value<bool>(smallDD.data[1].deviceSlaveID, 1, L"3.0.2");

			//Test 3.1 remove first element
			smallName[0] = 1;
			expected_value<bool>(smallDD.RemoveDevice(smallName, NULL, NULL), true, L"3.1.0");
			expected_value<bool>(smallDD.data[0].deviceType, 0, L"3.1.1");
			expected_value<bool>(smallDD.data[0].deviceSlaveID, 1, L"3.1.2");

			//Test 3.2 remove last element
			smallName[0] = 9;
			expected_value<bool>(smallDD.RemoveDevice(smallName, NULL, NULL), true, L"3.2.0");
			expected_value<bool>(smallDD.data[4].deviceType, 0, L"3.2.1");
			expected_value<bool>(smallDD.data[4].deviceSlaveID, 0, L"3.2.2");
			expected_value<bool>(smallDD.data[0].deviceSlaveID, 1, L"3.2.3");

			//Test 3.3 remove 4th element
			smallName[0] = 7;
			expected_value<bool>(smallDD.RemoveDevice(smallName, NULL, NULL), true, L"3.3.0");
			expected_value<bool>(smallDD.data[3].deviceType, 0, L"3.3.1");
			expected_value<bool>(smallDD.data[3].deviceSlaveID, 0, L"3.3.2");
			expected_value<bool>(smallDD.data[0].deviceSlaveID, 1, L"3.3.3");

			//Test 3.4 remove 3rd element
			smallName[0] = 5;
			expected_value<bool>(smallDD.RemoveDevice(smallName, NULL, NULL), true, L"3.4.0");
			expected_value<bool>(smallDD.data[2].deviceType, 0, L"3.4.1");
			expected_value<bool>(smallDD.data[2].deviceSlaveID, 0, L"3.4.2");
			expected_value<bool>(smallDD.data[0].deviceSlaveID, 0, L"3.4.3");

			//Test 3.4 remove non-existent element
			smallName[0] = 11;
			expected_value<bool>(smallDD.RemoveDevice(smallName, NULL, NULL), false, L"3.5.0");
		}
	};
}