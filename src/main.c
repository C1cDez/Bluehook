#include "shell.h"
#include "bluehook.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


static
void signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		printf("Interruption caught!\n");
		bluehook_cleanup();
		exit(0);
	}
}

int main(int argc, char** argv)
{
	if (bluehook_init()) return 1;

	signal(SIGINT, signal_handler);

	int res = 0;
	if (argc == 1) res = shell_start();
	else res = shell_execute(argc - 1, argv + 1);

	bluehook_cleanup();
	return res;
}
