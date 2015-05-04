#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

typedef char string[150];

int readFILE(char *,FILE*);
int prompt();

int main(char *envp[])
{
    char commandbuffer[512] = { '\0' } ;
    char* path = "./";
    int argc;
    char* argv[10];
    int *status;
    size_t nargs;
    char delim[] = "\n";
    FILE* fp;
    pid_t pid;
    pid_t opid;
    int pipepieces[2];
    int z;
    int count;
    int pgid = 0;

    for(;;){
        pipe(pipepieces);
        prompt();
        fflush(NULL);
        if(fgets(commandbuffer,sizeof(commandbuffer),stdin) == NULL){
            printf("ERROR :: bleh");
            return 0;
        }
        char* newline = strstr(commandbuffer,"\n");
        if(newline) *newline = 0;
        if(!strcmp(commandbuffer,"quit")) return 0;
        opid = fork();
        pipepieces[0]=dup(0);

        if(opid == 0){
            // check for command line input
            int i = 0;
            /* For debugging */

            char tokens[50][sizeof(commandbuffer)];
            char* token;
            token = strtok(commandbuffer,"|");
            int index = 0;
            while(token != '\0')
            {
                strcpy(tokens[index++],token);
                token = strtok(NULL,"|");

                /* debugging tokenizing of command from commandline*/
                // printf(" %s\n",token);
            }

            char temp[50][50];
            char bettertokens[50][512][50];
            char* bettertoken;
            int betterindex = 0;

            int j;

            char *arguments[50];

            for(i = 0 ; i < index; i++){
                bettertoken = strtok(tokens[i]," ");
                int argCount = 0;
                while(bettertoken != NULL){
                    arguments[argCount]=bettertoken;
                    bettertoken = strtok(NULL," ");
                    argCount++;
                }

                arguments[argCount] = 0;
                close(pipepieces[1]);
                close(0);
                dup(pipepieces[0]);
                close(pipepieces[0]);
                pipe(pipepieces);
                if(i!=index-1){
                    pid = fork();
                    if(pgid == 0) pgid = pid;
                    if(pid==0){
                        setpgid(0,pgid);
                        close(pipepieces[0]);
                        close(1);
                        dup(pipepieces[1]);
                        close(pipepieces[1]);
                        execvp(arguments[0],arguments);
                    }
                }
                else{
                    setpgid(0,pgid);
                    execvp(arguments[0],arguments);
                }
            }
        }
        else{
            wait(opid);
        }
    }
    return 0;
}

/*
 * prompt :: function that prints the shell's 'ush' prompt.
 */
int prompt()
{
    printf("ush> ");
    return 0;
}
