#include "shell.h"


int main(int argc, char** argv)
{
	if (argc == 1) return shell_start();
	else return shell_execute(argc - 1, argv + 1);
}
