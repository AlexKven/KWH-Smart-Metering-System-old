#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WConstants.h"
#endif

typedef void(*WriteCallback)  (const byte what);    // send a byte to serial port
typedef int(*AvailableCallback)  ();    // return number of bytes available
typedef int(*ReadCallback)  ();    // read a byte from serial port
typedef void(*TimeoutCallback)  ();    // send a byte to serial port
typedef void(*HandleMessageCallback) (bool error, byte *message, int length);
const byte STX = 0;
const byte ETX = 255;
const byte recipientLength = 8;

class Messenger
{
private:
	byte **messageQueue;
	byte *messageRecipients;
	byte *incomingMessage;
	int messageQueueSize;
	int messageQueueStart;

	void(*writeCB)(const byte what);
	byte(*availableCB)();
	byte(*readCB)();
	void(*handleMessageCB)(bool error, byte *message, int length);

	byte messengerId;
	byte *validBytes;
	byte currentRecipient[recipientLength];
	int messageSize;
	int maxMessageQueueSize;

	byte tryCorrectError(byte incoming, byte* validBytes, int numValidBytes);
	byte crc8(const byte *addr, byte len);
	void sendComplemented(void(*fSend)(const byte what), const byte what);

	int NumberOfSetBits(int i);

	bool have_stx = false;
	bool have_etx = false;
	byte input_pos;
	bool first_nibble;
	byte current_byte;
	bool error = false;
	bool isForMe = false;
	byte recipientPos = 0;
	long lastMsg = 0;

	void resetSendVariables();
public:
	Messenger();
	void initialize(byte _messengerId, int _messageSize, int _maxMessageQueueSize, void(*_writeCB)(const byte what), byte(*_availableCB)(), byte(*_readCB)()
		, void(*_handleMessageCB)(bool error, byte *message, int length));

	byte getMessengerId();

	void sendMsg(byte recipient, const byte * data);
	bool recvMsg();
	void recvDataBlock();

	void enqueueMsg(byte recipient, const byte *data);
	byte dequeueMsg(byte *data);

	bool canEnqueue();
	bool canDequeue();

	void sendMessageQueue();

	int getMessageSize();
	
	long getLastMsgTime();
	long setLastMsgTime(long val);
};