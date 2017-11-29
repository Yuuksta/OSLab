#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

void function_main();
void function1();
void function2();

int p1,p2,pp1,pp2;
int count = 0;
int pipefd[2];
int main (){
    int status;
    char messsage[20];
    pipe(pipefd);//initial pipe
    signal(SIGINT,function_main);
    if((p1 = fork()) == 0){  //first son process
        signal(SIGUSR1,function1);
        char str[20];
        while(1){
            sprintf(str,"%s%d%s","I send you ", count ," times");
            write(pipefd[1],str,20);
            count++;
            sleep(1);
        }
    }
    else if((p2 = fork()) == 0){ //second son process
        signal(SIGUSR2,function2);
            while(1){
            read(pipefd[0],messsage,20);
            printf("%s\n",messsage);
            sleep(1);
        }
    }
    else{
        waitpid(p1,&status,0);
        waitpid(p2,&status,0);
        close(pipefd[0]); //close pipe
        close(pipefd[1]);
        printf("Parent Process is Killed!\n");
        exit(0);
    }
}
void function1(){
    printf("Child Process 1 is Killed by Parent!\n");
    exit(0);
}

void function2(){
    printf("Child Process 2 is Killed by Parent!\n");
    exit(0);
}

void function_main(){
    kill(p1,SIGUSR1);
    kill(p2,SIGUSR2);
}