#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int x = 0;
      int w00t = 0;
        if((x = fork()) == 0)
          return 12;//exit(1);
        else
          waitpid(x, &w00t, 0);
        if(WIFEXITED(w00t))
          printf("%d\n", WEXITSTATUS(w00t));
    return 0;
}

