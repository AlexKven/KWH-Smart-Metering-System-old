#pragma once

using namespace std;

template<typename T>
void expected_value(T actual, T expected, wstring testName)
{
	bool result = (expected == actual);
	std::wostringstream ws;
	ws << testName << ":" << endl << "Expected = " << expected << endl << "Actual = " << actual;
	wstring s(ws.str());
	Assert::IsTrue(result, s.c_str());
}