#pragma once

typedef struct
{
	int timeout;
	char connected, authetificated, remembered, unknown;
	char do_info;
} bth_scan_query_t;

typedef struct
{
	char connectable, discoverable;   // 0 - do nothing, 1 - change to 1, 2 - change to 0
} bth_radio_query_t;

typedef struct
{
	char addr[18];
	int timeout;
	char mitm_protection_policy[4];
} bth_auth_query_t;
