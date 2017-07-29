#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\..\Libraries\DeviceDirectory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef DeviceDirectory<short, int, 30, 8> BigDeviceDirectory;
typedef DeviceDirectory<char, short, 10, 4> SmallDeviceDirectory;
typedef short BigDDName[8];
typedef char ShortDDName[4];

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
			BigDDName bigName;
			bigName[2] = 1;
			bigDD.AddDevice(1, 3, bigName);
			bigName[2] = 2;
			bigDD.AddDevice(1, 2, bigName);
			bigName[2] = 1;
			short type;
			short slaveID;
			Assert::IsTrue(bigDD.GetDeviceDetails(bigName, &type, &slaveID));
			Assert::IsTrue(type == 1);
			Assert::IsTrue(slaveID == 3);
		}

	};
}