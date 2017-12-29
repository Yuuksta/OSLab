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

int shmid[SHMLEN];
int semid,a = 0;
void P(int semid, int index);
void V(int semid, int index); 

struct stat statbuf;
int main (int argc,char** argv){
    printf("filename:%d",*argv[2]);
    int i,count = 1,filesize,size_count = 0;
    char *bufP[SHMLEN];
    for(i = 0;i < SHMLEN;i++){
        shmid[i] = shmget(SHMKEY+i,1024,IPC_CREAT|0666);
        bufP[i] = (char *)shmat(shmid[i],NULL,0);
    }
    //initial semid1
    if((semid = semget(SEMKEY, 3, IPC_CREAT|0666)) == -1){
        printf("semget failed\n");
        exit(0);
    }
    int fileID;//file ID
    if((fileID = open(argv[2], O_RDWR | O_CREAT,0666)) == -1){
        printf("file open error\n");
        exit(0);
    }
    stat(argv[1],&statbuf);
    filesize = statbuf.st_size;
    printf("filesize:%d\n",filesize);
    while(1){
        P(semid,1);
        P(semid,2);
        if(filesize - size_count < SINGLE_SIZE) size_count += write(fileID, bufP[count], filesize - size_count);
        else size_count += write(fileID, bufP[count], SINGLE_SIZE);
        printf("writen size: %d    writing........%s\n",size_count,bufP[count]);      
        V(semid,2);
        V(semid,0);
        if(size_count == filesize){
            printf("write finished\n");
            close(fileID);
            exit(0);           
        }
        count = (count + 1) % SHMLEN;
    }
    exit(0);
}

void P(int semid, int index){
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void V(int semid, int index){
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}