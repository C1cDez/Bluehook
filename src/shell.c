#include "shell.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "argquery.h"
#include "bluehook.h"


static
int shell_help(int argc, char** argv, int shellmode)
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
				"list ...\t\tLists all cached devices (use --cache in scan)\n"
				"info [addr] ...\t\tShows info about device\n"
				"remove [addr]\t\tRemoves authentification bewteen device and a computer\n"
				"pair, auth [addr] ...\tSends authentification request to device\n"
				"\n"
				"... - stands for more options. To see them type \"help 'command'\"\n"
			);
		}
		else
		{
			const char* subcommand = argv[1];
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
					"\t-i\t\tShows full information about the device\n"
					"\t--cache\t\tStore scanned data\n"
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
			else if (!strcmp("list", subcommand))
			{
				printf(
					"list\n"
					"\t-i\t\tShow full information about device\n"
				);
			}
			else if (!strcmp("info", subcommand))
			{
				printf(
					"info\n"
					"\t-lc\t\tForce to use local cache data rather than scan\n"
				);
			}
			else
				printf("No additional context provided for '%s'\n", subcommand);
		}
	}
	else
	{
		printf(
			"bluehook is a CLI utility to manage Bluetooth devices & connections\n"
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
			"\t\t-i\t\t\t\tShows full information about the device\n"
			"\t\t--cache\t\t\t\tStore scanned data\n"
			"\n"
			"\t--list\t\t\t\tShows cached devices\n"
			"\t\t-i\t\t\t\tShows full information about the device\n"
			"\n"
			"\t--info [addr]\t\t\tShows info about device\n"
			"\t\t-lc\t\t\t\tForce to use local cache data rather than scan\n"
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

static
int contains_arg(int argc, char** argv, const char* key)
{
	for (int i = 0; i < argc; i++)
		if (!strncmp(argv[i], key, strlen(key))) return i;
	return -1;
}

static
int shell_radio(int argc, char** argv)
{
	bth_radio_query_t radio_query = {
		.connectability = 0,
		.discoverability = 0
	};
	if (argc >= 2)
	{
		if (contains_arg(argc, argv, "-c") != -1)		radio_query.connectability = 1;
		else if (contains_arg(argc, argv, "-nc") != -1)	radio_query.connectability = 2;
		if (contains_arg(argc, argv, "-d") != -1)		radio_query.discoverability = 1;
		else if (contains_arg(argc, argv, "-nd") != -1)	radio_query.discoverability = 2;
	}

	return bluehook_radio_info(&radio_query);
}

static
int shell_scan(int argc, char** argv)
{
	bth_scan_query_t scan_query = {
		.timeout = 10,
		.connected = 1,
		.authetificated = 1,
		.remembered = 1,
		.unknown = 1,
		.do_info = 0,
		.do_cache = 0
	};
	if (contains_arg(argc, argv, "-c") != -1) scan_query.connected = 0;
	if (contains_arg(argc, argv, "-a") != -1) scan_query.authetificated = 0;
	if (contains_arg(argc, argv, "-r") != -1) scan_query.remembered = 0;
	if (contains_arg(argc, argv, "-u") != -1) scan_query.unknown = 0;
	if (contains_arg(argc, argv, "-i") != -1) scan_query.do_info = 1;
	if (contains_arg(argc, argv, "--cache") != -1) scan_query.do_cache = 1;

	int time_arg = contains_arg(argc, argv, "-t=");
	if (time_arg != -1)
	{
		char* time = argv[time_arg] + 3;
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

static
int shell_info(int argc, char** argv)
{
	ADDRESS_EXPECTED(argc, "to get info from");
	bth_info_query_t info_query = {
		.addr = { 0 },
		.force_lc = 0
	};
	memcpy(info_query.addr, argv[1], min(17, strlen(argv[1])));

	if (contains_arg(argc, argv, "-lc") != -1) info_query.force_lc = 1;

	return bluehook_device_info(&info_query);
}

static
int shell_remove(int argc, char** argv)
{
	ADDRESS_EXPECTED(argc, "to be removed");
	return bluehook_remove(argv[1]);
}

static
int shell_pair(int argc, char** argv)
{
	ADDRESS_EXPECTED(argc, "to pair");

	bth_auth_query_t auth_query = {
		.addr = { 0 },
		.timeout = 30,
		.mitm_protection_policy = { 0 }
	};
	memcpy(auth_query.addr, argv[1], min(17, strlen(argv[1])));

	int time_arg = contains_arg(argc, argv, "-t=");
	if (time_arg != -1)
	{
		char* time = argv[time_arg] + 3;
		auth_query.timeout = atoi(time);
	}

	int mitm_arg = contains_arg(argc, argv, "-m=");
	if (mitm_arg != -1)
	{
		char* policy = argv[mitm_arg] + 3;
		memcpy(auth_query.mitm_protection_policy, policy, 3);
	}

	return bluehook_auth(&auth_query);
}

static
int shell_list(int argc, char** argv)
{
	bth_list_query_t list_query = {
		.info = 0,
		.ioop = -1
	};
	if (contains_arg(argc, argv, "-i") != -1) list_query.info = 1;
	return bluehook_list(&list_query);
}


static
int tokenize_line(char* line, char** tokens)
{
	int i = 0;
	char* next_token = NULL;
	char* token = strtok_s(line, " ", &next_token);
	while (token != NULL)
	{
		tokens[i] = token;
		i++;
		token = strtok_s(NULL, " ", &next_token);
	}
	return i;
}
int shell_start()
{
	system("title Bluehook");

	char inputline[256] = { 0 };
	while (1)
	{
		printf("\nbluehook>");
		gets_s(inputline, sizeof(inputline));
		char* argv[64] = { NULL };
		int argc = tokenize_line(inputline, argv);

		if (argc == 0 || !argv[0]) continue;

		const char* subcommand = argv[0];
		if (!strcmp("exit", subcommand) || !strcmp("quit", subcommand)) break;
		else if (!strcmp("help", subcommand) || !strcmp("?", subcommand)) shell_help(argc, argv, 1);
		else if (!strcmp("clear", subcommand) || !strcmp("cls", subcommand)) system("cls");
		else if (!strcmp("radio", subcommand)) shell_radio(argc, argv);
		else if (!strcmp("scan", subcommand)) shell_scan(argc, argv);
		else if (!strcmp("list", subcommand)) shell_list(argc, argv);
		else if (!strcmp("info", subcommand)) shell_info(argc, argv);
		else if (!strcmp("remove", subcommand)) shell_remove(argc, argv);
		else if (!strcmp("pair", subcommand) || !strcmp("auth", subcommand)) shell_pair(argc, argv);
		else printf("Unrecognized command: '%s'\n", subcommand);
	}

	return 0;
}

int shell_execute(int argc, char** argv)
{
	const char* subcommand = argv[0];
	if (!strcmp("--help", subcommand) || !strcmp("-h", subcommand))
		return shell_help(argc, argv, 0);
	else if (!strcmp("--radio", subcommand)) return shell_radio(argc, argv);
	else if (!strcmp("--scan", subcommand)) return shell_scan(argc, argv);
	else if (!strcmp("--list", subcommand)) return shell_list(argc, argv);
	else if (!strcmp("--info", subcommand)) return shell_info(argc, argv);
	else if (!strcmp("--remove", subcommand)) return shell_remove(argc, argv);
	else if (!strcmp("--pair", subcommand) || !strcmp("--auth", subcommand)) 
		return shell_pair(argc, argv);
	else
	{
		printf("Unrecognized command: '%s'\n", subcommand);
		return 1;
	}
}
