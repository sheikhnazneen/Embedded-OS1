/*Write a program to embed an array to a shared memory containing int values.
Read the array from another process increament value of the array in second process*/

#include<stdio.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<pthread.h>


#define SIZE 5

int main(){
    key_t key=1234;
    int shmid;
    int *shared_array;

    shmid =shmget(key,SIZE *sizeof(int),0666 | IPC_CREAT);
    if(shmid == -1){
        perror("shmget");
        exit(1);
    } 
    pid_t pid = fork();
    if(pid<0){
        perror("fork");
        exit(1);
    }
    if(pid == 0){
        shared_array =(int *)shmat(shmid,NULL,0);
        if(shared_array == (int *)-1){
            perror("shmat");
            exit(1);
        }
        printf("Chlid Process: Increamenting values in  shared memory.\n");
        for(int i=0;i<SIZE;i++){
            shared_array[i] +=1;
        }
        printf("Child Process:Updated array values: \n");
        for(int i=0;i<SIZE;i++){
            printf("shared_array[%d]=%d\n",i,shared_array[i]);
        }
        if(shmdt(shared_array)==-1){
            perror("shmdt");
            exit(1);
        }
        exit(0);
    }else{
        shared_array =(int *)shmat(shmid,NULL,0);
        if(shared_array == (int *)-1){
            perror("shmat");
            exit(1);
        }
        printf("Parent Process: Initializing array in shared memory.\n");
        for(int i =0;i<SIZE;i++){
            shared_array[1]=i*10;
        }
        printf("Parent Process: Initial array values:\n");
                for(int i =0;i<SIZE;i++){
         printf("shared_array[%d]= %d\n",i,shared_array[i]);
    }
    wait(NULL);
    if(shmctl(shmid,IPC_RMID,NULL)==-1){
        perror("shmctl");
        exit(1);
    }
    printf("Parent Process:Shared memory cleaned up.\n");
}
return 0;
}