#include "shell.h"
#include "bluehook.h"


int main(int argc, char** argv)
{
	if (bluehook_init()) return 1;

	int res = 0;
	if (argc == 1) res = shell_start();
	else res = shell_execute(argc - 1, argv + 1);

	bluehook_cleanup();
	return res;
}
