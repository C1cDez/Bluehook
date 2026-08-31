#pragma once

#include "argquery.h"


int bluehook_init(void);
int bluehook_cleanup(void);

int bluehook_radio_info(bth_radio_query_t query);
int bluehook_scan(bth_scan_query_t query);
int bluehook_device_info(bth_info_query_t addr);

int bluehook_remove(const char *addr);
int bluehook_auth(bth_auth_query_t query);
int bluehook_ping(bth_ping_query_t query);
