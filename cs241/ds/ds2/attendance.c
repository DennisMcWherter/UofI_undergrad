/*
  This program tracks the seat number and names of students in class.
  Comments marked with TODO need to be completed in order for the code to
  be complete.
*/


#include <stdio.h>
#include <stdlib.h>

typedef enum _status_code {
    SUCCESS = 0,
    FAILURE = 1
} status_code;

typedef enum _cmds {
    INSERT = 1,
    PRINT = 2
} cmds;

static int MAX_INPUT_LENGTH = 50;
static int NUMBER_OF_SEATS = 22;

static char **seats;

void cleanup_memory(){
    int idx;
    for(idx=0;idx < NUMBER_OF_SEATS;idx++){
        //TODO free and clear the array seats
        free(seats[idx]);
    }
    free(seats);
}

int main(){
    char cmd[MAX_INPUT_LENGTH];
    int choice = 0;
    int number = 0;
    int length = 0;
    char * name = 0;
    
    seats = malloc(NUMBER_OF_SEATS * sizeof(int));
    
    while(1){
        printf("Please enter a number to run command:\n\t1: Insert\n\t2: Print\n");
        if(0 == fgets(cmd, MAX_INPUT_LENGTH, stdin)){
            cleanup_memory();
            return FAILURE;
        }
        /*TODO
        Convert the string to an integer and 
        return to prompt if it is invalid
        int choice = 
        */
        choice = *(int*)cmd;
        if(choice == INSERT){
            printf("Please enter the seat number:");
            //TODO get an integer from user
            scanf("%d", &choice);
            
            if(number < 0 || NUMBER_OF_SEATS <= number){
                printf("Invalid seat number\n");
                continue;
            }
            printf("Please enter a name:");
            //TODO get a name for the seat, hint: need to use malloc
            length = scanf("%s", name);
            name[length] = '\0';
            
            seats[number] = name;
        }else if(choice == PRINT){
            int index;
            printf("Current Seating Arrangement\n");
            for(index=0; index < NUMBER_OF_SEATS;++index){
                if(seats[index] == NULL){
                    printf("Seat %d is empty.\n", index);
                }else{
                    printf("Seat %d is filled by %s\n", index, seats[index]);
                }
            }
        }else{
            //TODO print warning to user and return to prompt
        }
    }
    
}
