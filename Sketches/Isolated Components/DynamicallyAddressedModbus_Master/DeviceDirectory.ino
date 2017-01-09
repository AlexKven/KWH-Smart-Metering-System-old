const int MAX_DEVICES = 256;
uint8_t* DeviceDirectory;

//Device Directory row: [Name (8), DeviceType (1), SlaveID (1)]
//If empty, DeviceType = 0, and SlaveID = 0 unless there's
//device entries after that row, then SlaveID = 1

void InitializeDeviceDirectory()
{
  DeviceDirectory = new uint8_t[MAX_DEVICES * 10];
  for (int i = 0; i < MAX_DEVICES * 10; i++)
    DeviceDirectory[i] = 0;
}

int FindFreeRow()
{
  int row = 0;
  while (DeviceDirectory[row * 10 + 8] != 0)
    row++;
  return row;
}

int FindFreeSlaveID()
{
  int slaveID = 1;
  int ind;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    ind = 10 * i;
    if (DeviceDirectory[ind + 8] == 0)
    {
      if (DeviceDirectory[ind + 9] == 0)
        return slaveID;
    }
    if (DeviceDirectory[ind + 9] == slaveID)
    {
      slaveID++;
      i = 0;
      if (slaveID > 247)
        return 0;
    }
  }
  return slaveID;
}

int AddToDeviceDirectory(uint8_t* devName8, uint8_t devType, int slaveID)
{
  int row = FindFreeRow();
  InsertIntoDeviceDirectoryRow(row, devName8, devType, slaveID);
  return row;
}

bool FindDeviceDetails(uint8_t* devName8, uint8_t *devTypeOut, uint8_t *slaveIDOut, int *rowOut)
{
  *devTypeOut = 0;
  *slaveIDOut = 0;
  *rowOut = -1;
  int ind;
  bool found;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    ind = 10 * i;
    if (DeviceDirectory[ind + 8] == 0)
    {
      if (DeviceDirectory[ind + 9] == 0)
        return false;
    }
    else
    {
      bool found = true;
      for (int j = 0; j < 8; j++)
      {
        if (devName8[j] != DeviceDirectory[ind +j])
          found = false;
      }
      if (found)
      {
        *devTypeOut = DeviceDirectory[ind + 8];
        *slaveIDOut = DeviceDirectory[ind + 9];
        *rowOut = i;
        return true;
      }
    }
  }
  return false;
}

bool FindDeviceDetails(uint8_t* devName8, uint8_t *devTypeOut, uint8_t *slaveIDOut)
{
  int dummy;
  return FindDeviceDetails(devName8, devTypeOut, slaveIDOut, &dummy);
}

int DeleteDevicesForSlaveNotInList(uint8_t** devName8s, int devName8sCount, uint8_t slaveID)
{
  int numDeleted = 0;
  int ind;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    ind = 10 * i;
    if (DeviceDirectory[ind + 8] == 0)
    {
      if (DeviceDirectory[ind + 9] == 0)
        return numDeleted;
    }
    else if (DeviceDirectory[ind + 9] == slaveID)
    {
      bool found = false;
      for (int k = 0; k < devName8sCount; k++)
      {
        uint8_t* curName = devName8s[k];
        bool match = true;
        for (int j = 0; j < 8; j++)
        {
          if (curName[j] != DeviceDirectory[ind +j])
            match = false;
        }
        if (match)
          found = true;
      }
      if (!found)
      {
        ClearDeviceDirectoryRow(i);
        numDeleted++;
      }
    }
  }
  numDeleted++;
}

void InsertIntoDeviceDirectoryRow(int row, uint8_t* devName8, uint8_t devType, int slaveID)
{
  int ind = 10 * row;
  for (int i = 0; i < 8; i++)
    DeviceDirectory[ind + i] = devName8[i];
  DeviceDirectory[ind + 8] = devType;
  DeviceDirectory[ind + 9] = slaveID;
}

bool UpdateItemInDeviceDirectory(uint8_t* devName8, uint8_t devType, int slaveID)
{
  uint8_t curType;
  uint8_t curID;
  int row;
  FindDeviceDetails(devName8, &curType, &curID, &row);
  if (row >= 0 && (curType != devType || curID != slaveID))
  {
    InsertIntoDeviceDirectoryRow(row, devName8, devType, slaveID);
    return true;
  }
  return false;
}

void ClearDeviceDirectoryRow(int row)
{
  int ind = 10 * row;
  for (int i = 0; i < 9; i++)
    DeviceDirectory[ind + i] = 0;
  if (row == MAX_DEVICES - 1 || DeviceDirectory[ind + 18] == 0)
    DeviceDirectory[ind + 9] = 0;
  else
    DeviceDirectory[ind + 9] = 1;
  if (row > 0 && DeviceDirectory[ind - 2] == 0)
    DeviceDirectory[ind - 1] = 0;
}
