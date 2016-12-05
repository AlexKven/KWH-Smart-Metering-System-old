#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WConstants.h"
#endif
class Messenger;

class Secretary
{
private:
	Messenger *messenger;

	int serialTxControl;
	int serialDataEnable;

	byte *curMessage;

	bool myTurn = false;

	bool master = false;

	void SetWriteMode(bool writeEnable);
public:
	void handleMessage(bool error, byte *message, int len);
	void initialize(Messenger &_messenger, int _serialTxControl, int _serialDataEnable, bool _master);

	Secretary();
	~Secretary();
	void loop();

	void sendUsageData(byte recipient, unsigned long time, unsigned long usage);
	void sendUsageData(byte recipient, unsigned long time, unsigned long usage0, unsigned long usage1);
	void requestUsageData(byte recipient, unsigned long time, unsigned long length);
	void requestTime(byte recipient);
	void announceTime(unsigned long time);
	
	bool receiveUsageData(byte *message, byte *sender, unsigned long *time, unsigned long *usage0, unsigned long *usage1, bool *twoPoints);
	bool receiveUsageRequest(byte *message, unsigned long *time, int *length);
	bool receiveTimeRequest(byte *message);
	bool receiveTimeAnnouncement(byte *message, unsigned long *time);
};