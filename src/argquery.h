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
	char do_cache;
} bth_scan_query_t;

typedef struct
{
	info_query_params_t iqp;
	// 0 - Load
	// 1 - Store
	char ioop;
} bth_list_query_t;

typedef struct
{
	// 0 - do nothing
	// 1 - change to TRUE 
	// 2 - change to FALSE
	char connectability, discoverability;
	info_query_params_t iqp;
} bth_radio_query_t;

typedef struct
{
	char addr[18];
	char force_lc;
	info_query_params_t iqp;
} bth_info_query_t;

typedef struct
{
	char addr[18];
	int timeout;
	char mitm_protection_policy[4];
} bth_auth_query_t;
