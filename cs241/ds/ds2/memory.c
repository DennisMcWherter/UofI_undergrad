/*
  Please remove my errors and test using valgrind
  gcc memory.c
  valgrind ./a.out
*/

#include <stdlib.h>
#include <string.h>

/*static will make the integer have file-scope rather than program scope*/
static int ALLOCATION_BLOCK_SIZE = 50; //Bytes


/*this is a definition, not a declaration*/
typedef enum _status_code {
    SUCCESS = 0,
    FAILURE = 1
} status_code;

int main(){
    int i = 0;
    char *block = malloc(ALLOCATION_BLOCK_SIZE);
    if(i == 0){
        memset(block, 'A', ALLOCATION_BLOCK_SIZE);
    }else{
        return FAILURE;
    }
    free(block);
    return SUCCESS;
}
