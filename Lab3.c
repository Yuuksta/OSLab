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
int semid,a = 0;
int pipefd[2];

struct stat statbuf;
int main (int argc,char** argv){
    int p1,p2,i;
    pipe(pipefd);//initial pipe
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
        writebuf(argv);
    }
    else if((p2 = fork()) == 0){
        readbuf(argv);
    }
    else{
        wait(0);
        wait(0);
        for(i = 0;i < SHMLEN;i++){
            shmctl(shmid[i],IPC_RMID,0);
        }
        exit(0);
    }
}

void readbuf(char** argv){
    int i,count = 1,filesize,size_count = 0;
    char *bufP[SHMLEN];
    for(i = 0;i < SHMLEN;i++){
        bufP[i] = (char *)shmat(shmid[i],NULL,0);
        //printf("readbuf:%d\n",bufP[i]);
    }			
    int fileID;//file ID
    if((fileID=open(argv[1], O_RDONLY)) == 2){
        printf("file open error\n");
        exit(0);
    }
    stat(argv[1],&statbuf);
    filesize = statbuf.st_size;
    //printf("%d",filesize);
    write(pipefd[1],&filesize,4);
    while(1){
        P(semid,0);
        P(semid,2);
        size_count += read(fileID,bufP[count],SINGLE_SIZE);
        printf("read size: %d    reading.........%s\n",size_count,bufP[count]);        
        V(semid,2);
        V(semid,1);
        if(size_count == filesize){
            printf("read finished\n");
            close(fileID);
            exit(0);
        }
        count = (count + 1) % SHMLEN;
    }
    exit(0);
}

void writebuf(char** argv){
    int i,count = 1,filesize,size_count = 0;
    char *bufP[SHMLEN];
    for(i = 0;i < SHMLEN;i++){
        bufP[i] = (char *)shmat(shmid[i],NULL,0);
        //printf("writebuf:%s\n",bufP[i]);        
    }
    int fileID;//file ID
    if((fileID = open(argv[2], O_RDWR | O_CREAT,0666)) == 2){
        printf("file open error\n");
        exit(0);
    }
    read(pipefd[0],&filesize,4);
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