#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("pid before fork = %d\n", getpid());

	pid_t child_pid = fork();

	if (child_pid < 0) {		
		// error code
		perror("Fork Failed");
		return 1;
	
	}
	else if (child_pid == 0) {		
		// child code
		sleep(1);
		printf("%d is the child of parent process %d.\n", getpid(), getppid());
	
	} 
	else { /* child_pid > 0 */		
		// parent code
		sleep(2);
		printf("%d is the parent of child process %d.\n", getpid(), child_pid);
	
	}
	return 0;
}

