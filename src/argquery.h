#pragma once

typedef struct
{
	int timeout;
	char connected, authetificated, remembered, unknown;
	char do_info;
} bth_scan_query_t;

typedef struct
{
	char connectable, discoverable;
} bth_radio_query_t;

typedef struct
{
	char addr[18];
} bth_auth_query_t;
