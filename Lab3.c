#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHMKEY 36
#define SHMLEN 10
#define SINGLE_SIZE 100
#define SEMKEY 2222

void P(int semid, int index);
void V(int semid, int index);
void readbuf(char** argv);
void writebuf(char** argv);

int shmid[SHMLEN];
int semid;
struct stat statbuf;

int main (int argc,char** argv){
    int p1,p2,i;
    //initial shared memory
    for(i = 0;i < SHMLEN;i++){
        shmid[i] = shmget(SHMKEY+i,1024,IPC_CREAT|0666);
    }
    //initial semid1
    if((semid = semget(SEMKEY, 3, IPC_CREAT|0666)) == -1){
        printf("semget failed\n");
    }
    semctl(semid, 0, SETVAL, SHMLEN);//judge if buffer is full
    semctl(semid, 1, SETVAL, 0);//or empty
    semctl(semid, 2, SETVAL, 1);//mutex
    if((p1 = fork()) == 0){
        execv("./writebuf",argv);
    }
    else if((p2 = fork()) == 0){
        execv("./readbuf",argv);
    }
    else{
        wait(0);
        wait(0);
        for(i = 0;i < SHMLEN;i++){ //remove shared memory
            shmctl(shmid[i],IPC_RMID,0);
        }
        semctl(semid,3,IPC_RMID,NULL); //remove sem
        exit(0);
    }
}