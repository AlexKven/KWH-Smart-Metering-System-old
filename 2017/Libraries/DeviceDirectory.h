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

	bool compareNames(T_byte name1[S_name], T_byte name2[S_name])
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

	bool GetDeviceDetails(T_byte deviceName[S_name], T_byte *deviceType_out, T_byte *deviceSlaveID_out)
	{
		T_int row = 0;
		Entry entry = data[row];
		while ((entry.deviceType != 0 || entry.deviceSlaveID == 1) && row < S_directory)
		{
			if (entry.deviceType != 0 && compareNames(entry.deviceName, deviceName))
			{
				safelySetOut(deviceType_out, entry.deviceType);
				safelySetOut(deviceSlaveID_out, entry.deviceSlaveID);
				return true;
			}
			row++;
			entry = data[row];
		}
		return false;
	}

	bool RemoveDevice(T_byte deviceName[S_name], T_byte *deviceType_out, T_byte *deviceSlaveID_out)
	{
		T_int row = 0;
		Entry entry = data[row];
		while ((entry.deviceType != 0 || entry.deviceSlaveID == 1) && row < S_directory)
		{
			if (entry.deviceType != 0 && compareNames(entry.deviceName, deviceName))
			{
				safelySetOut(deviceType_out, entry.deviceType);
				safelySetOut(deviceSlaveID_out, entry.deviceSlaveID);

				entry.deviceType = 0;
				data[row] = entry;
				setLastEmptyIndicators(row);

				return true;
			}
			row++;
			entry = data[row];
		}
		return false;
	}
};