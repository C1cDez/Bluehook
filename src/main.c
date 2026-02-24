#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "argquery.h"
#include "bluehook.h"

static
int contain_arg(int argc, char** argv, const char* key)
{
	for (int i = 0; i < argc; i++)
		if (!strncmp(argv[i], key, strlen(key))) return i;
	return -1;
}

static
int execute(int argc, char** argv)
{
	if (!is_bluetooth_available())
	{
		printf("Bluetooth is turned off\nEnable it on ms-settings:bluetooth\n");
		return 1;
	}

	const char* subcommand = argv[0];
	if (!strcmp("--radio", subcommand))
	{
		bth_radio_query_t radio_query = { 0 };
		if (argc >= 2)
		{
			if (contain_arg(argc, argv, "-c") != -1) radio_query.connectable = 1;
			if (contain_arg(argc, argv, "-d") != -1) radio_query.discoverable = 1;
		}
		bluehook_radio_info(&radio_query);
	}
	else if (!strcmp("--scan", argv[0]))
	{
		bth_scan_query_t scan_query = {
			.timeout			= 10,
			.connected			= 1,
			.authetificated		= 1,
			.remembered			= 1,
			.unknown			= 1,
			.do_info			= 0
		};
		if (contain_arg(argc, argv, "-c") != -1) scan_query.connected		= 0;
		if (contain_arg(argc, argv, "-a") != -1) scan_query.authetificated	= 0;
		if (contain_arg(argc, argv, "-r") != -1) scan_query.remembered		= 0;
		if (contain_arg(argc, argv, "-u") != -1) scan_query.unknown			= 0;
		if (contain_arg(argc, argv, "-i") != -1) scan_query.do_info			= 1;

		int time_arg = contain_arg(argc, argv, "-t=");
		if (time_arg != -1)
		{
			char* time = argv[time_arg];
			time += 3;
			scan_query.timeout = atoi(time);
		}

		return bluehook_scan(&scan_query);
	}
	else if (!strcmp("--info", subcommand))
	{
		if (argc <= 1)
		{
			fprintf(stderr, "Expected address XX:XX:XX:XX:XX:XX of device to be removed\n");
			return 1;
		}
		bluehook_device_info(argv[1]);
	}
	else if (!strcmp("--remove", subcommand))
	{
		if (argc <= 1)
		{
			fprintf(stderr, "Expected address XX:XX:XX:XX:XX:XX of device to be removed\n");
			return 1;
		}
		bluehook_remove(argv[1]);
	}
	else if (!strcmp("--pair", subcommand) || !strcmp("--auth", subcommand))
	{
		if (argc <= 1)
		{
			fprintf(stderr, "Expected address XX:XX:XX:XX:XX:XX of device to be paired\n");
			return 1;
		}
		bth_auth_query_t auth_query = {
			.addr = { 0 }
		};
		memcpy(auth_query.addr, argv[1], min(17, strlen(argv[1])));
		bluehook_auth(&auth_query);
	}
	else
	{
		printf("Unrecognized command: '%s'\n", subcommand);
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	if ((argc == 2 && !strcmp("--help", argv[1])) || argc == 1)
	{
		printf(
			"%s is a CLI utility to manage Bluetooth devices & connections\n"
			"\nUsage:\n"
			"\t--radio\t\t\tShows info about your bluetooth radio\n"
			"\t\t-c\t\t\tFlip radio's connectable state\n"
			"\t\t-d\t\t\tFlip radio's discoverable state\n"
			"\t--scan\t\t\tScans local area for bluetooth devices\n"
			"\t\t-t=[s]\t\t\tSet timeout, in seconds\n"
			"\t\t-c\t\t\tDON'T look for Connected devices\n"
			"\t\t-a\t\t\tDON'T look for Authentificated (paired) devices\n"
			"\t\t-r\t\t\tDON'T look for Remebered devices\n"
			"\t\t-u\t\t\tDON'T look for Unknown devices\n"
			"\t\t-i\t\t\tShows full information about the device\n"
			"\t--info [addr]\t\tShows info about device\n"
			"\t--remove [addr]\t\tRemoves authentification bewteen device and a computer\n"
			"\t--pair [addr]\t\tSends authentification request to device\n"
			,
			argv[0]
		);
	}
	else return execute(argc - 1, argv + 1);
}
