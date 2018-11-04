#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>



char line[1025] ;
char *argvv[500][3];
char *arg[1024];
int p=0;
void splitpip() {
	
	char *ch;
	char pipcheck[]={'|'};
	char* rest = line;
	char* token;
	while ((token = strtok_r(rest, "|", &rest))){ 
		arg[p]=token;
		p++; 
		}
}
void splitspace() {
	int i;
	char st[1024];
	char spacecheck[]={' '};
	for (i=0;i<p;i++){
		if (strpbrk(arg[i],spacecheck)){
			argvv[i][0] = strtok(arg[i], " ");
			}
		else{
			argvv[i][0] = strtok(arg[i], "\n");
			}
		argvv[i][1] = strtok(NULL,"\n");
		argvv[i][1] = strtok(argvv[i][1]," ");
		argvv[i][2]=NULL;
	}
	
}


int m=0;

void execute() {
    int numPipes = p-1;
    int status;
    int i = 0;
    pid_t pid;

    int pipefds[2*numPipes];

    for(i = 0; i < (numPipes); i++){
        if(pipe(pipefds + i*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }


    int j = 0;
    while(argvv[m][0]) {
        pid = fork();
        if(pid == 0) {

            //if not last command
            if(argvv[m+1][0]){
                if(dup2(pipefds[j + 1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            //if not first command&& j!= 2*numPipes
            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror(" dup2");///j-2 0 j+1 1
                    exit(EXIT_FAILURE);

                }
            }


            for(i = 0; i < 2*numPipes; i++){
                    close(pipefds[i]);
            }

            if( execvp(*argvv[m], argvv[m]) < 0 ){
			printf("command not found");
                    exit(EXIT_FAILURE);
            }
        } else if(pid < 0){
            perror("error");
            exit(EXIT_FAILURE);
        }

        m++;
        j+=2;
    }
    /**Parent closes the pipes and wait for children*/

    for(i = 0; i < 2 * numPipes; i++){
        close(pipefds[i]);
    }

    for(i = 0; i < numPipes + 1; i++)
        wait(&status);
}


int main(void) {
	while(1==1){
	m=0;
	p=0;
	printf("\nuser >>");
	fgets(line, sizeof(line), stdin);
	splitpip();
	splitspace();
	execute();
	}
		}
