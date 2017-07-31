#pragma once

template<typename T_byte, typename T_int, T_int S_directory, T_byte S_name>
class DeviceDirectory
{
#if UNIT_TEST
public:
#else
private:
#endif
	struct Entry
	{
		T_byte deviceType = 0;
		T_byte deviceSlaveID = 0;
		T_byte deviceName[S_name];
		//deviceType = 0 && deviceSlaveID == 0: No device, past end of list
		//deviceType = 0 && deviceSlaveID == 1: No device, not at end of list
	};

#if UNIT_TEST
	int testCounter = 0;
#endif

	void copyName(T_byte destination[S_name], T_byte source[S_name])
	{
		for (int i = 0; i < S_name; i++)
		{
			destination[i] = source[i];
		}
	}

	template<typename T>
	void safelySetOut(T *out_param, T value)
	{
		if (out_param != NULL)
			*out_param = value;
	}

	Entry data[S_directory];

	bool compareNames(T_byte *name1, T_byte *name2)
	{
		for (int i = 0; i < S_name; i++)
		{
			if (name1[i] != name2[i])
				return false;
		}
		return true;
	}

	void setLastEmptyIndicators(int row)
	{
		Entry entry = data[row];
		bool notLast = (row < S_directory - 1 && (data[row + 1].deviceType != 0 || data[row + 1].deviceSlaveID != 0));
		if (entry.deviceType == 0)
		{
			if (notLast)
				entry.deviceSlaveID = 1;
			else
				entry.deviceSlaveID = 0;
			data[row] = entry;
		}
		row--;
		entry = data[row];
		while (row >= 0 && entry.deviceType == 0 && ((entry.deviceSlaveID == 1) != notLast))
		{
			if (notLast)
				entry.deviceSlaveID = 1;
			else
				entry.deviceSlaveID = 0;
			data[row] = entry;
			row--;
			entry = data[row];
		}
	}
public:
	bool AddDevice(T_byte deviceType, T_byte deviceSlaveID, T_byte deviceName[S_name])
	{
		T_int row = 0;
		while ((data[row].deviceType != 0 || data[row].deviceSlaveID == 1) && row < S_directory)
			row++;
		if (row == S_directory)
			return false;
		Entry entry;
		entry.deviceType = deviceType;
		entry.deviceSlaveID = deviceSlaveID;
		copyName(entry.deviceName, deviceName);
		data[row] = entry;
		return true;
	}

//	bool GetDeviceDetails(T_byte deviceName[S_name], T_byte *deviceType_out, T_byte *deviceSlaveID_out)
//	{
//		T_int row = 0;
//		Entry entry = data[row];
//		while ((entry.deviceType != 0 || entry.deviceSlaveID == 1) && row < S_directory)
//		{
//			if (entry.deviceType != 0 && compareNames(entry.deviceName, deviceName))
//			{
//				safelySetOut(deviceType_out, entry.deviceType);
//				safelySetOut(deviceSlaveID_out, entry.deviceSlaveID);
//				return true;
//			}
//			row++;
//			entry = data[row];
//#if UNIT_TEST
//			testCounter++;
//#endif
//		}
//		return false;
//	}

//	bool RemoveDevice(T_byte deviceName[S_name], T_byte *deviceType_out, T_byte *deviceSlaveID_out)
//	{
//		T_int row = 0;
//		Entry entry = data[row];
//		while ((entry.deviceType != 0 || entry.deviceSlaveID == 1) && row < S_directory)
//		{
//			if (entry.deviceType != 0 && compareNames(entry.deviceName, deviceName))
//			{
//				safelySetOut(deviceType_out, entry.deviceType);
//				safelySetOut(deviceSlaveID_out, entry.deviceSlaveID);
//
//				entry.deviceType = 0;
//				data[row] = entry;
//				setLastEmptyIndicators(row);
//				return true;
//			}
//			row++;
//			entry = data[row];
//#if UNIT_TEST
//			testCounter++;
//#endif
//		}
//		return false;
//	}

	bool RemoveDevice(T_int row)
	{
		if (row == S_directory || row < 0)
			return false;
		Entry entry = data[row];
		entry.deviceType = 0;
		data[row] = entry;
		setLastEmptyIndicators(row);
		return true;
	}

	T_int searchNext(T_int searchStart, T_byte *deviceType_inout, T_byte *slaveID_inout, T_byte (*deviceName_inout)[S_name], bool searchByDeviceType, bool searchBySlaveID, bool searchByDeviceName)
	{
		T_int row = searchStart;
		if (row == S_directory)
			return row;
		Entry entry = data[row];
		while ((entry.deviceType != 0 || entry.deviceSlaveID == 1) && row < S_directory)
		{
			bool match = true;
			if (match && searchBySlaveID)
				match = (slaveID_inout == NULL || entry.deviceSlaveID == *slaveID_inout);
			if (match && searchByDeviceType)
				match = (deviceType_inout == NULL || entry.deviceType == *deviceType_inout);
			if (match && searchByDeviceName)
			{
				if (deviceName_inout == NULL)
					match = true;
				else
				{
					T_byte name[S_name];
					copyName(name, *deviceName_inout);
					match = compareNames(entry.deviceName, name);
				}
			}
			if (match)
			{
				if (!searchBySlaveID)
					safelySetOut(slaveID_inout, entry.deviceSlaveID);
				if (!searchByDeviceType)
					safelySetOut(deviceType_inout, entry.deviceType);
				if (!searchByDeviceName && deviceName_inout != NULL)
					copyName(*deviceName_inout, entry.deviceName);
				return row;
			}
			row++;
			entry = data[row];
#if UNIT_TEST
			testCounter++;
#endif
		}
		return S_directory;
	}
};