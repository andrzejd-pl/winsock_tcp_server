#include "CurrentTime.h"

std::string currentTime() {
	SYSTEMTIME st;
	GetSystemTime(&st);
	char buffer[256];
	sprintf(buffer,
			"%d.%02d.%02d %02d:%02d:%02d.%03d",
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds);

	std::string currentTime = buffer;
	return currentTime;
}