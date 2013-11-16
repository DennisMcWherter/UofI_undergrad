#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int result;

	pid_t child_pid = fork();
	if (child_pid < 0) {		
		// error code
		perror("Fork Failed");
		return 1;

	} 
	else if (child_pid == 0) {		
		// child code
		sleep(2);
		printf("%d is the child of parent process %d.\n", getpid(), getppid());
	}
	else { 
		// parent code
		sleep(1);

		if (waitpid(child_pid, &result, 0) == -1) {
			perror("Wait failed");
			return -1;
		}
		printf("%d is the parent of child process %d.\n", getpid(), child_pid);
	}
	return 0;
}

