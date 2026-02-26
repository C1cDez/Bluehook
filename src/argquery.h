#pragma once

typedef struct
{
	int timeout;
	char connected, authetificated, remembered, unknown;
	char do_info;
} bth_scan_query_t;

typedef struct
{
	// 0 - do nothing
	// 1 - change to TRUE 
	// 2 - change to FALSE
	char connectability, discoverability;
} bth_radio_query_t;

typedef struct
{
	char addr[18];
	int timeout;
	char mitm_protection_policy[4];
} bth_auth_query_t;
