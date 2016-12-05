/*
RS485 protocol library.

Devised and written by Nick Gammon.
Date: 14 November 2011
Version: 1.1

Version 1.1 reset the timeout period after getting STX.

Licence: Released for public use.


Can send from 1 to 255 bytes from one node to another with:

* Packet start indicator (STX)
* Each data byte is doubled and inverted to check validity
* Packet end indicator (ETX)
* Packet CRC (checksum)


To allow flexibility with hardware (eg. Serial, SoftwareSerial, I2C)
you provide three "callback" functions which send or receive data. Examples are:

void fWrite (const byte what)
{
Serial.write (what);
}

int fAvailable ()
{
return Serial.available ();
}

int fRead ()
{
return Serial.read ();
}

*/


#include <Messenger.h>

// calculate 8-bit CRC
byte Messenger::crc8(const byte *addr, byte len)
{
	byte crc = 0;
	while (len--)
	{
		byte inbyte = *addr++;
		for (byte i = 8; i; i--)
		{
			byte mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix)
				crc ^= 0x8C;
			inbyte >>= 1;
		}  // end of for
	}  // end of while
	return crc;
}  // end of crc8

	 // send a byte complemented, repeated
	 // only values sent would be (in hex): 
	 //   0F, 1E, 2D, 3C, 4B, 5A, 69, 78, 87, 96, A5, B4, C3, D2, E1, F0
void Messenger::sendComplemented(void(*fSend)(const byte what), const byte what)
{
	byte c;

	// first nibble
	c = what >> 4;
	fSend((c << 4) | (c ^ 0x0F));

	// second nibble
	c = what & 0x0F;
	fSend((c << 4) | (c ^ 0x0F));

}  // end of sendComplemented

	 // send a message of "length" bytes (max 255) to other end
	 // put STX at start, ETX at end, and add CRC
void Messenger::sendMsg(byte recipient, const byte * data)
{
	writeCB(STX);  // STX
	for (byte i = 0; i < recipientLength; i++)
		writeCB(recipient);
	for (byte i = 0; i < messageSize; i++)
		sendComplemented(writeCB, data[i]);
	writeCB(ETX);  // ETX
	sendComplemented(writeCB, crc8(data, messageSize));
}  // end of sendMsg

void Messenger::initialize(byte _messengerId, int _messageSize, int _maxMessageQueueSize, void(*_writeCB)(const byte what), byte(*_availableCB)(), byte(*_readCB)()
	, void(*_handleMessageCB)(bool error, byte *message, int length))
{
	writeCB = _writeCB;
	availableCB = _availableCB;
	readCB = _readCB;
	handleMessageCB = _handleMessageCB;
	messengerId = _messengerId;
	messageSize = _messageSize;
	messageQueueSize = 0;
	messageQueueStart = 0;
	maxMessageQueueSize = _maxMessageQueueSize;
	validBytes = new byte[18];
	validBytes[0] = STX;
	validBytes[1] = ETX;
	validBytes[2] = 0x0F;
	validBytes[3] = 0x1E;
	validBytes[4] = 0x2D;
	validBytes[5] = 0x3C;
	validBytes[6] = 0x4B;
	validBytes[7] = 0x5A;
	validBytes[8] = 0x69;
	validBytes[9] = 0x78;
	validBytes[10] = 0x87;
	validBytes[11] = 0x96;
	validBytes[12] = 0xA5;
	validBytes[13] = 0xB4;
	validBytes[14] = 0xC3;
	validBytes[15] = 0xD2;
	validBytes[16] = 0xE1;
	validBytes[17] = 0xF0;

	incomingMessage = new byte[messageSize];
	messageQueue = new byte*[maxMessageQueueSize];
	messageRecipients = new byte[maxMessageQueueSize];
	for (int i = 0; i < maxMessageQueueSize; i++)
	{
		messageQueue[i] = new byte[messageSize];
	}
}

void Messenger::resetSendVariables()
{
	have_stx = false;
	have_etx = false;
	error = false;
	isForMe = false;
	recipientPos = 0;
	input_pos = 0;
}

Messenger::Messenger() {}

	 // receive a message, maximum "length" bytes, timeout after "timeout" milliseconds
	 // if nothing received, or an error (eg. bad CRC, bad data) return 0
	 // otherwise, returns length of received data
bool Messenger::recvMsg()          // milliseconds before timing out
{
	bool processed = false;
	byte inByte = readCB();
	byte corrected = tryCorrectError(inByte, validBytes, 18);

	if (have_stx)
	{
		processed = true;
		if (recipientPos < recipientLength)
		{
			currentRecipient[recipientPos++] = inByte;
			if (recipientPos == recipientLength)
			{
				byte numMatches = 0;
				byte numBroadcast = 0;
				for (int i = 0; i < recipientLength; i++)
				{
					if (currentRecipient[i] == messengerId)
						numMatches++;
					if (currentRecipient[i] == 255)
						numBroadcast++;
				}
				isForMe = (numMatches >= recipientLength / 2) || (numBroadcast >= recipientLength / 2);
			}
		}
		else if (corrected == ETX)
		{
			have_etx = true;
		}
		else
		{
			if (inByte != corrected)
			{
				error = true;
				Serial.println("Er0");
			}
			// wait until packet officially starts

			//// check byte is in valid form (4 bits followed by 4 bits complemented)
			//if ((inByte >> 4) != ((inByte & 0x0F) ^ 0x0F))
			//	error = true;

			//inByte = tryCorrectError(inByte, validBytes, 18);

			// convert back 
			inByte >>= 4;

			// high-order nibble?
			if (first_nibble)
			{
				current_byte = inByte;
				first_nibble = false;
			}  // end of first nibble
			else
			{
				// low-order nibble
				current_byte <<= 4;
				current_byte |= inByte;
				first_nibble = true;
			}

			// if we have the ETX this must be the CRC
			if (have_etx && first_nibble)
			{
				processed = true;
				have_stx = false;
				if (crc8(incomingMessage, input_pos) != current_byte && isForMe)
				{
					error = true;
					Serial.println("Er1");
				}
				if (isForMe)
					handleMessageCB(error, incomingMessage, input_pos);  // return received length
				resetSendVariables();
				return true;
			}  // end if have ETX already

				 // keep adding if not full
			if (first_nibble)
				lastMsg = millis();
			if (isForMe && first_nibble)
			{
				if (input_pos < messageSize)
					incomingMessage[input_pos++] = current_byte;
				else
				{
					handleMessageCB(true, incomingMessage, input_pos);
					resetSendVariables();
					return true;
				}
			}
		}
	}
	else if (corrected == STX)
	{
		have_stx = true;
		have_etx = false;
		processed = true;
		input_pos = 0;
		first_nibble = true;
	}
	//switch (corrected)
	//{

	//case STX:   // start of text
	//	have_stx = true;
	//	have_etx = false;
	//	input_pos = 0;
	//	first_nibble = true;
	//	start_time = millis();  // reset timeout period
	//	break;

	//case ETX:   // end of text
	//	have_etx = true;
	//	break;

	//default:
	//	if (!have_stx)
	//		break;

	//	
	//	}
	//}  // end of switch
	if (!processed)
		resetSendVariables();
	return processed;
} // end of recvMsg

int Messenger::NumberOfSetBits(int i)
{
	// Java: use >>> instead of >>
	// C or C++: use uint32_t
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

byte Messenger::tryCorrectError(byte incoming, byte* validBytes, int numvalidBytes)
{
	int curNumDiffs = 9;
	int numDiffs = 0;
	byte curByte = 0;
	for (int i = 0; i < numvalidBytes; i++)
	{
		numDiffs = 0;
		byte diffs = incoming ^ validBytes[i];
		numDiffs = (byte)NumberOfSetBits(diffs);
		if (numDiffs < curNumDiffs)
		{
			curNumDiffs = numDiffs;
			curByte = validBytes[i];
		}
	}
	return curByte;
}

int Messenger::getMessageSize()
{
	return messageSize;
}

bool Messenger::canEnqueue()
{
	return messageQueueSize < maxMessageQueueSize;
}

bool Messenger::canDequeue()
{
	return messageQueueSize > 0;
}

byte Messenger::dequeueMsg(byte *data)
{
	byte recipient = messageRecipients[messageQueueStart];
	for (int i = 0; i < messageSize; i++)
	{
		data[i] = messageQueue[messageQueueStart][i];
	}
	messageQueueStart++;
	messageQueueSize--;
	if (messageQueueStart == maxMessageQueueSize)
		messageQueueStart = 0;
	return recipient;
}

void Messenger::enqueueMsg(byte recipient, const byte *data)
{
	byte pos = (messageQueueSize + messageQueueStart) % maxMessageQueueSize;
	messageQueueSize++;
	messageRecipients[pos] = recipient;
	for (int i = 0; i < messageSize; i++)
	{
		messageQueue[pos][i] = data[i];
	}
}

void Messenger::sendMessageQueue()
{
	byte recipient;
	while (canDequeue())
	{
		recipient = dequeueMsg(incomingMessage);
		sendMsg(recipient, incomingMessage);
	}
}

byte Messenger::getMessengerId()
{
	return messengerId;
}

void Messenger::recvDataBlock()
{
	while (availableCB())
	{
		recvMsg();
	}
}

long Messenger::getLastMsgTime()
{
	return lastMsg;
}

long Messenger::setLastMsgTime(long val)
{
	lastMsg = val;
}