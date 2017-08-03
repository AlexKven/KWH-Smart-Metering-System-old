#pragma once

template<typename T_byte, typename T_int, typename T_ulong, T_byte S_name>
class Communicator
{
#if UNIT_TEST
public:
#else
private:
#endif
	virtual void initializeInternal() = 0;

public:
	virtual void loop(T_ulong millis) = 0;
};