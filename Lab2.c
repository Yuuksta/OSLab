#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/sem.h>

void P(int semid, int index);
void V(int semid, int index);
void *subp1();
void *subp2();

int semid1,semid2,a = 0;
pthread_t p1,p2;

int main(){
    int ret;
    union semun arg1,arg2;
    arg1.val = 1;
    arg2.val = 0;
    if((semid1 = semget(6666, 1, IPC_CREAT|0666)) == -1){
        printf("semget failed\n");
    }
    if((semid2 = semget(6665, 1, IPC_CREAT|0666)) == -1){
        printf("semget failed\n");
    }
    semctl(semid2, 0, SETVAL, arg2);
    semctl(semid1, 0, SETVAL, arg1);
    if((ret = pthread_create(&p2,NULL,subp2,NULL) != 0)){
        return -1;
    }
    if((ret = pthread_create(&p1,NULL,subp1,NULL) != 0)){
        return -1;
    }
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    semctl(semid1,0,IPC_RMID,NULL);
    semctl(semid2,0,IPC_RMID,NULL);
}

void *subp1(){
    for(int i = 1;i <= 100; i++){
        P(semid1,0);
        a += i;
        V(semid2,0);        
    }
    return;
}

void *subp2(){
    for(int i = 1;i <= 100; i++){
        P(semid2,0);
        printf("%d ",a);
        if(i%10 == 0){
            printf("\n");
        }
        V(semid1,0);
    }
    return;
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
