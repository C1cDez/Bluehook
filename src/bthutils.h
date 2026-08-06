#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <BluetoothAPIs.h>
#include <BluetoothLEAPIs.h>

#pragma comment(lib, "Bthprops.lib")
#pragma comment(lib, "WS2_32.lib")


#define BLUETOOTH_ADDRESS_STRLEN 18

#define YESNO(cond) (cond) ? "Yes" : "No"


void format_systemtime(char* buff, SYSTEMTIME st);

void addr2str(char* buff, BLUETOOTH_ADDRESS_STRUCT addr);
BLUETOOTH_ADDRESS_STRUCT str2addr(const char* addr);
