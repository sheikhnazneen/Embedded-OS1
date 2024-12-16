#include<stdio.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#define SHM_NAME "desd_shm"
#define SIZE 4096

int main(){
    int shm_fd = shm_open(SHM_NAME,O_RDONLY,0666);
    if(shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    char *shm_ptr =mmap(NULL,SIZE,PROT_READ,MAP_SHARED,shm_fd,0);
    if(shm_ptr==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    printf("MESSAGE READ: %s\n",shm_ptr);
    close(shm_fd);
    if(shm_unlink(SHM_NAME)==-1){
        perror("shm_unlink");
        exit(1);
    }
}