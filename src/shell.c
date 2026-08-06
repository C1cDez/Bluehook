#include "shell.h"

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "argquery.h"
#include "bluehook.h"


static int shell_help(int argc, char **argv, int shellmode)
{
	if (shellmode)
	{
		if (argc == 1)
		{
			printf(
				"exit, quit\t\tExits shell mode\n"
				"clear, cls\t\tClears shell screen\n"
				"help\t\t\tShows this screen\n"
				"radio ...\t\tShows info about your bluetooth radio\n"
				"scan ...\t\tScans local area for bluetooth devices\n"
				"info [addr]\t\tShows info about device\n"
				"remove [addr]\t\tRemoves authentification bewteen device and a computer\n"
				"pair, auth [addr] ...\tSends authentification request to device\n"
				"\n"
				"... - stands for more options. To see them type \"help 'command'\"\n"
			);
		}
		else
		{
			const char *subcommand = argv[1];
			if (!strcmp("radio", subcommand))
			{
				printf(
					"radio\n"
					"\t-c, -nc\t\tMakes radio connectable/not-connectable\n"
					"\t-d, -nd\t\tMakes radio discoverable/not-discoverable\n"
					"\n"
					"\tP.S.:\n"
					"\t\tChange in the discoverability state requires radio to be CONNECTABLE\n"
					"\t\tMaking radio NOT-CONNECTABLE requires it to be NOT-DISCOVERABLE\n"
				);
			}
			else if (!strcmp("scan", subcommand))
			{
				printf(
					"scan\n"
					"\t-t=[s]\t\tSet timeout, in seconds\n"
					"\t-c\t\tDO NOT look for Connected devices\n"
					"\t-a\t\tDO NOT look for Authentificated (paired) devices\n"
					"\t-r\t\tDO NOT look for Remembered devices\n"
					"\t-u\t\tDO NOT look for Unknown devices\n"
				);
			}
			else if (!strcmp("pair", subcommand) || !strcmp("auth", subcommand))
			{
				printf(
					"pair, auth [addr]\n"
					"\t-t=[s]\t\t\t\tSet timeout, in seconds\n"
					"\t-m=[r|rb|rg|nr|nrb|nrg]\t\tSpecifies MITM protection policy\n"
					"\t\tr\t\t\t\t- Required\n"
					"\t\trb\t\t\t\t- Required bonding\n"
					"\t\trg\t\t\t\t- Required general bonding\n"
					"\t\tnr\t\t\t\t- Not Required\n"
					"\t\tnrb\t\t\t\t- Not Required bonding\n"
					"\t\tnrg\t\t\t\t- Not Required general bonding\n"
				);
			}
			else
				printf("No additional context provided for '%s'\n", subcommand);
		}
	}
	else
	{
		printf(
			"bluehook is a CLI utility to manage Bluetooth devices & connections.\n"
			"\nUsage:\n"
			"\t--radio\t\t\t\tShows info about your bluetooth radio\n"
			"\t\t-c, -nc\t\t\t\tMakes radio connectable/not-connectable\n"
			"\t\t-d, -nd\t\t\t\tMakes radio discoverable/not-discoverable (SHELL MODE ONLY)\n"
			"\n"
			"\t--scan\t\t\t\tScans local area for bluetooth devices\n"
			"\t\t-t=[s]\t\t\t\tSet timeout, in seconds\n"
			"\t\t-c\t\t\t\tDO NOT look for Connected devices\n"
			"\t\t-a\t\t\t\tDO NOT look for Authentificated (paired) devices\n"
			"\t\t-r\t\t\t\tDO NOT look for Remebered devices\n"
			"\t\t-u\t\t\t\tDO NOT look for Unknown devices\n"
			"\n"
			"\t--info [addr]\t\t\tShows info about device\n"
			"\n"
			"\t--remove [addr]\t\t\tRemoves authentification bewteen device and a computer\n"
			"\n"
			"\t--pair [addr] <-t> <-m>\t\tSends authentification request to device\n"
			"\t\t-t=[s]\t\t\t\tSet timeout, in seconds\n"
			"\t\t-m=[r|rb|rg|nr|nrb|nrg]\t\tSpecifies MITM protection policy\n"
			"\t\t\tr\t\t\t\t- Required\n"
			"\t\t\trb\t\t\t\t- Required bonding\n"
			"\t\t\trg\t\t\t\t- Required general bonding\n"
			"\t\t\tnr\t\t\t\t- Not Required\n"
			"\t\t\tnrb\t\t\t\t- Not Required bonding\n"
			"\t\t\tnrg\t\t\t\t- Not Required general bonding\n"
		);
	}
	return 0;
}

static int contains_arg(int argc, char **argv, const char *key)
{
	for (int i = 0; i < argc; i++)
		if (!strncmp(argv[i], key, strlen(key))) return i;
	return -1;
}

static info_query_params_t get_info_query_params(int argc, char **argv)
{
	info_query_params_t iqp = { 0, 0 };
	if (contains_arg(argc, argv, "-i") != -1)	iqp.do_info = 1;
	if (contains_arg(argc, argv, "-hs") != -1)	iqp.hide_services = 1;
	return iqp;
}

static int shell_radio(int argc, char **argv)
{
	bth_radio_query_t radio_query = {
		.connectability = 0,
		.discoverability = 0,
		.iqp = get_info_query_params(argc, argv)
	};
	radio_query.iqp.do_info = 1;
	if (argc >= 2)
	{
		if (contains_arg(argc, argv, "-c") != -1)		radio_query.connectability = 1;
		else if (contains_arg(argc, argv, "-nc") != -1)	radio_query.connectability = 2;
		if (contains_arg(argc, argv, "-d") != -1)		radio_query.discoverability = 1;
		else if (contains_arg(argc, argv, "-nd") != -1)	radio_query.discoverability = 2;
	}

	return bluehook_radio_info(&radio_query);
}

static int shell_scan(int argc, char **argv)
{
	bth_scan_query_t scan_query = {
		.timeout = 10,
		.connected = 1,
		.authetificated = 1,
		.remembered = 1,
		.unknown = 1,
		.iqp = get_info_query_params(argc, argv),
	};
	if (contains_arg(argc, argv, "-c") != -1) scan_query.connected = 0;
	if (contains_arg(argc, argv, "-a") != -1) scan_query.authetificated = 0;
	if (contains_arg(argc, argv, "-r") != -1) scan_query.remembered = 0;
	if (contains_arg(argc, argv, "-u") != -1) scan_query.unknown = 0;

	int time_arg = contains_arg(argc, argv, "-t=");
	if (time_arg != -1)
	{
		char *time = argv[time_arg] + 3;
		scan_query.timeout = atoi(time);
	}

	return bluehook_scan(&scan_query);
}


#define ADDRESS_EXPECTED(argc, reason) do { \
	if (argc <= 1) \
	{ \
		fprintf(stderr, "Expected address XX:XX:XX:XX:XX:XX of device " reason "\n"); \
		return 1; \
	} \
} while (0);

static int shell_info(int argc, char **argv)
{
	ADDRESS_EXPECTED(argc, "to get info from");
	bth_info_query_t info_query = {
		.addr = { 0 },
		.iqp = get_info_query_params(argc, argv)
	};
	info_query.iqp.do_info = 1;
	memcpy(info_query.addr, argv[1], min(sizeof(info_query.addr) - 1, strlen(argv[1])));

	return bluehook_device_info(&info_query);
}

static int shell_remove(int argc, char **argv)
{
	ADDRESS_EXPECTED(argc, "to be removed");
	return bluehook_remove(argv[1]);
}

static int shell_pair(int argc, char **argv)
{
	ADDRESS_EXPECTED(argc, "to pair");

	bth_auth_query_t auth_query = {
		.addr = { 0 },
		.timeout = -1,
		.mitm_protection_policy = { 0 }
	};
	memcpy(auth_query.addr, argv[1], min(sizeof(auth_query.addr) - 1, strlen(argv[1])));

	int time_arg = contains_arg(argc, argv, "-t=");
	if (time_arg != -1)
	{
		char *time = argv[time_arg] + 3;
		auth_query.timeout = atoi(time);
	}

	int mitm_arg = contains_arg(argc, argv, "-m=");
	if (mitm_arg != -1)
	{
		char *policy = argv[mitm_arg] + 3;
		memcpy(auth_query.mitm_protection_policy, policy, 3);
	}

	return bluehook_auth(&auth_query);
}


static int tokenize_line(char *line, char **tokens, int toklimit)
{
	int i = 0;
	char *token = strtok(line, " ");
	while (token && i < toklimit)
	{
		tokens[i] = token;
		token = strtok(NULL, " ");
		++i;
	}
	return i;
}
int shell_start(void)
{
	system("title Bluehook");

	char inputline[256] = { 0 };
	while (1)
	{
		printf("bluehook>");
		if (!fgets(inputline, sizeof(inputline), stdin)) break;
		inputline[strcspn(inputline, "\n")] = '\0';

		char *argv[64] = { NULL };
		int argc = tokenize_line(inputline, argv, __crt_countof(argv));

		if (argc == 0 || !argv[0]) continue;

		const char *subcommand = argv[0];
		if (!strcmp("exit", subcommand) || !strcmp("quit", subcommand))
			break;
		else if (!strcmp("help", subcommand) || !strcmp("?", subcommand))
			shell_help(argc, argv, 1);
		else if (!strcmp("clear", subcommand) || !strcmp("cls", subcommand))
			system("cls");
		else if (!strcmp("radio", subcommand))
			shell_radio(argc, argv);
		else if (!strcmp("scan", subcommand))
			shell_scan(argc, argv);
		else if (!strcmp("info", subcommand))
			shell_info(argc, argv);
		else if (!strcmp("remove", subcommand))
			shell_remove(argc, argv);
		else if (!strcmp("pair", subcommand) || !strcmp("auth", subcommand))
			shell_pair(argc, argv);
		else printf("Unrecognized command: '%s'\n", subcommand);

		putchar('\n');
	}

	return 0;
}

int shell_execute(int argc, char **argv)
{
	const char *subcommand = argv[0];
	if (!strcmp("--help", subcommand) || !strcmp("-h", subcommand))
		return shell_help(argc, argv, 0);
	else if (!strcmp("--radio", subcommand))
		return shell_radio(argc, argv);
	else if (!strcmp("--scan", subcommand))
		return shell_scan(argc, argv);
	else if (!strcmp("--info", subcommand))
		return shell_info(argc, argv);
	else if (!strcmp("--remove", subcommand))
		return shell_remove(argc, argv);
	else if (!strcmp("--pair", subcommand) || !strcmp("--auth", subcommand))
		return shell_pair(argc, argv);
	else
	{
		printf("Unrecognized command: '%s'\n", subcommand);
		return 1;
	}
}
