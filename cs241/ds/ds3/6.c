#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int result;

	chdir("newdir");

	// execute pwd from a new process
	system("pwd");

	return 0;
}

