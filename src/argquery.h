#pragma once

typedef struct
{
	char do_info, hide_services;
} info_query_params_t;

typedef struct
{
	int timeout;
	char connected, authetificated, remembered, unknown;
	info_query_params_t iqp;
	char inquiry;
} bth_scan_query_t;

typedef struct
{
	/* 0 - do nothing */
	/* 1 - change to TRUE */
	/* 2 - change to FALSE */
	char connectability, discoverability;
	info_query_params_t iqp;
} bth_radio_query_t;

#define BLUETOOTH_ADDRESS_STRLEN 18
typedef struct
{
	char addr[BLUETOOTH_ADDRESS_STRLEN];
	info_query_params_t iqp;
} bth_info_query_t;

typedef struct
{
	char addr[BLUETOOTH_ADDRESS_STRLEN];
	int timeout;
	char mitm_protection_policy[4];
} bth_auth_query_t;

typedef struct
{
	char addr[BLUETOOTH_ADDRESS_STRLEN];
	int amount;
	int timeout;
} bth_ping_query_t;
