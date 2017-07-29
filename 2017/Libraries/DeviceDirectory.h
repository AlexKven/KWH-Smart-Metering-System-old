#pragma once

template<typename T_byte, typename T_int, T_byte S_name, T_int S_directory>
class DeviceDirectory
{
private:
	struct Entry
	{
		T_byte deviceType = 0;
		T_byte deviceSlaveID = 0;
		T_byte deviceName[S_name];
		//deviceType = 0 && deviceSlaveID == 0: No device, past end of list
		//deviceType = 0 && deviceSlaveID == 1: No device, not at end of list
	};

	void copyName(T_byte destination[S_name], T_byte source[S_name])
	{
		for (int i = 0; i < S_name; i++)
		{
			destination[i] = source[i];
		}
	}

	Entry data[S_directory];
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
	}

	bool GetDeviceDetails(T_byte deviceName[S_name], T_byte *deviceType_out, T_byte *deviceSlaveID_out)
	{
		T_int row = 0;
		Entry entry = data[row];
		while ((entry.deviceType != 0 || entry.deviceSlaveID == 1) && row < S_directory)
		{
			if (entry.deviceType != 0 && compareNames(entry.deviceName, deviceName))
			{
				*deviceType_out = entry.deviceType;
				*deviceSlaveID_out = entry.deviceSlaveID;
				return true;
			}
			row++;
			entry = data[row];
		}
		return false;
	}

	bool compareNames(T_byte name1[S_name], T_byte name2[S_name])
	{
		for (int i = 0; i < S_name; i++)
		{
			if (name1[i] != name2[i])
				return false;
		}
		return true;
	}
};