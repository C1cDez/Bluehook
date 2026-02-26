#pragma once

#include "argquery.h"


int is_bluetooth_available();

int bluehook_radio_info(bth_radio_query_t* query);
int bluehook_scan(bth_scan_query_t* query);
int bluehook_device_info(const char* addr);

int bluehook_remove(const char* addr);
int bluehook_auth(bth_auth_query_t* auth_query);
