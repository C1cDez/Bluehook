#include "bthutils.h"

#include <string.h>
#include <stdio.h>


void format_systemtime(char* buff, SYSTEMTIME st)
{
	if (st.wYear == 1601)
		sprintf(buff, "[NONE]");
	else
		sprintf(buff, "%hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

void addr2str(char* buff, BLUETOOTH_ADDRESS_STRUCT addr)
{
	sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x",
		addr.rgBytes[5], addr.rgBytes[4], addr.rgBytes[3], addr.rgBytes[2], addr.rgBytes[1], addr.rgBytes[0]);
}

BLUETOOTH_ADDRESS_STRUCT str2addr(const char* addr)
{
	unsigned b0, b1, b2, b3, b4, b5;
	int scan = sscanf(addr, "%2x:%2x:%2x:%2x:%2x:%2x", &b5, &b4, &b3, &b2, &b1, &b0);
	if (scan != 6) return (BLUETOOTH_ADDRESS_STRUCT) { .ullLong = BLUETOOTH_NULL_ADDRESS };

	BLUETOOTH_ADDRESS_STRUCT bth_addr = {
		.rgBytes = { b0, b1, b2, b3, b4, b5 }
	};
	return bth_addr;
}
