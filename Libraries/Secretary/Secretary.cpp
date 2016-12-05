#include "Secretary.h"
#include "../Messenger/Messenger.h"
#define RS485Transmit    HIGH
#define RS485Receive     LOW

// MESSAGE_ID GUIDE:
// 0 = pass talking stick
// 1 = data for single minute
// 2 = data for two minutes (even first minute)
// 3 = data for two minutes (odd first minute)
// 4 = data for single hour
// 5 = data for two hours (even first hour)
// 6 = data for two hours (odd first hour)

Secretary::Secretary() {}

void Secretary::initialize(Messenger &_messenger, int _serialTxControl, int _serialDataEnable, bool _master)
{
	messenger = &_messenger;
	curMessage = new byte[20];
	serialTxControl = _serialTxControl;
	serialDataEnable = _serialDataEnable;
	pinMode(_serialTxControl, OUTPUT);
	pinMode(_serialDataEnable, OUTPUT);
	digitalWrite(_serialTxControl, HIGH);
	digitalWrite(_serialDataEnable, HIGH);
	master = _master;
	SetWriteMode(false);
}

Secretary::~Secretary()
{
	delete messenger;
}

void Secretary::handleMessage(bool error, byte *message, int len)
{
	if (!error && message[9] == 0 && (message[10] == messenger->getMessengerId()))
		myTurn = true;
	else if (error || message[9] != 0)
	{
		if (error)
			Serial.println("Error Message!");
	}
}

void Secretary::SetWriteMode(bool writeEnable)
{
	if (writeEnable)
	{
		digitalWrite(serialTxControl, RS485Transmit);
		digitalWrite(serialDataEnable, RS485Transmit);
	}
	else
	{
		digitalWrite(serialTxControl, RS485Receive);
		digitalWrite(serialDataEnable, RS485Receive);
	}
}

void Secretary::loop()
{
	long curTime = millis();
	if (master && (curTime - messenger->getLastMsgTime() > 5000))
	{
		myTurn = true;
		messenger->setLastMsgTime(curTime);
	}
	//Serial.println(curTime - lastMsg);
	messenger->recvDataBlock();
	if (myTurn)
	{
		SetWriteMode(true);
		messenger->sendMessageQueue();
		curMessage[9] = 0;
		curMessage[10] = messenger->getMessengerId() + 1;
		messenger->sendMsg(255, curMessage);
		myTurn = false;
		//delay(400);
		SetWriteMode(false);
	}
}

void Secretary::sendUsageData(byte recipient, unsigned long time, unsigned long usage)
{
	curMessage[8] = 0;
	curMessage[10] = messenger->getMessengerId();
	curMessage[9] = 1;
	for (int i = 0; i < 4; i++)
	{
		curMessage[11 + i] = (time >> 24 - 8 * i);
		//Serial.println(curMessage[11 + i]);
	}
	for (int i = 0; i < 4; i++)
	{
		curMessage[15 + i] = (usage >> 24 - 8 * i);
		//Serial.println(curMessage[15 + i]);
	}
	messenger->enqueueMsg(recipient, curMessage);
}

void Secretary::sendUsageData(byte recipient, unsigned long time, unsigned long usage0, unsigned long usage1)
{

	curMessage[8] = 0;
	curMessage[10] = messenger->getMessengerId();
	if (time % 2 == 0)
	{
		curMessage[9] = 2;
	}
	else
	{
		curMessage[9] = 3;
	}
	unsigned long newTime = time >> 1;
	for (int i = 0; i < 3; i++)
	{
		curMessage[11 + i] = (newTime >> 16 - 8 * i);
		//Serial.println(curMessage[11 + i]);
	}
	for (int i = 0; i < 3; i++)
	{
		curMessage[14 + i] = (usage0 >> 16 - 8 * i);
	}
	for (int i = 0; i < 3; i++)
	{
		curMessage[17 + i] = (usage1 >> 16 - 8 * i);
	}
	messenger->enqueueMsg(recipient, curMessage);
}

void Secretary::requestUsageData(byte recipient, unsigned long time, unsigned long length)
{
	curMessage[8] = 0;
	curMessage[9] = 7;
	curMessage[10] = 0;
	for (int i = 0; i < 4; i++)
	{
		curMessage[11 + i] = (time >> 24 - 8 * i);
		//Serial.println(curMessage[11 + i]);
	}
	for (int i = 0; i < 4; i++)
	{
		curMessage[15 + i] = (length >> 24 - 8 * i);
		//Serial.println(curMessage[15 + i]);
	}
	messenger->enqueueMsg(recipient, curMessage);
}

void Secretary::requestTime(byte recipient)
{
	curMessage[8] = 0;
	curMessage[9] = 8;
	messenger->enqueueMsg(recipient, curMessage);
}

void Secretary::announceTime(unsigned long time)
{
	curMessage[8] = 0;
	curMessage[9] = 9;
	for (int i = 0; i < 4; i++)
	{
		curMessage[10 + i] = (time >> 24 - 8 * i);
	}
	messenger->enqueueMsg(255, curMessage);
}

bool Secretary::receiveUsageData(byte *message, byte *sender, unsigned long *time, unsigned long *usage0, unsigned long *usage1, bool *twoPoints)
{
	unsigned long _time = 0;
	unsigned long _usage = 0;
	*sender = message[10];
	switch (message[9])
	{
		case 1:
		for (int i = 0; i < 4; i++)
		{
			_time = _time << 8;
			_time += message[11 + i];
		}
		for (int i = 0; i < 4; i++)
		{
			_usage = _usage << 8;
			_usage += message[15 + i];
		}
		*time = _time;
		*usage0 = _usage;
		*twoPoints = false;
		return true;
		case 2:
		for (int i = 0; i < 3; i++)
		{
			_time = _time << 8;
			_time += message[11 + i];
		}
		_time *= 2;
		*time = _time;
		for (int i = 0; i < 3; i++)
		{
			_usage = _usage << 8;
			_usage += message[14 + i];
		}
		*usage0 = _usage;
		_usage = 0;
		for (int i = 0; i < 3; i++)
		{
			_usage = _usage << 8;
			_usage += message[17 + i];
		}
		*usage1 = _usage;
		*twoPoints = true;
		return true;
		case 3:
		for (int i = 0; i < 3; i++)
		{
			_time = _time << 8;
			_time += message[11 + i];
		}
		_time *= 2;
		_time++;
		*time = _time;
		for (int i = 0; i < 3; i++)
		{
			_usage = _usage << 8;
			_usage += message[14 + i];
		}
		*usage0 = _usage;
		_usage = 0;
		for (int i = 0; i < 3; i++)
		{
			_usage = _usage << 8;
			_usage += message[17 + i];
		}
		*usage1 = _usage;
		*twoPoints = true;
		return true;
	}
	return false;
}

bool Secretary::receiveUsageRequest(byte *message, unsigned long *time, int *length)
{
	unsigned long _time = 0;
	int _length = 0;
	if (message[9] == 7)
	{
		for (int i = 0; i < 4; i++)
		{
			_time = _time << 8;
			_time += message[11 + i];
		}
		for (int i = 0; i < 4; i++)
		{
			_length = _length << 8;
			_length += message[15 + i];
		}
		*time = _time;
		*length = _length;
		return true;
	}
	return false;
}

bool Secretary::receiveTimeRequest(byte *message)
{
	return message[9] == 8;
}

bool Secretary::receiveTimeAnnouncement(byte *message, unsigned long *time)
{
	unsigned long _time = 0;
	if (message[9] == 9)
	{
		for (int i = 0; i < 4; i++)
		{
			_time = _time << 8;
			_time += message[10 + i];
		}
		*time = _time;
		return true;
	}
	return false;
}