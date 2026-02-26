#pragma once

#include <Windows.h>
#include <BluetoothAPIs.h>

int devlist_init();
int devlist_load();

int devlist_next(BLUETOOTH_DEVICE_INFO_STRUCT* device);
int devlist_rewind();
int devlist_add(BLUETOOTH_DEVICE_INFO_STRUCT* device);

int devlist_store();
int devlist_cleanup();
