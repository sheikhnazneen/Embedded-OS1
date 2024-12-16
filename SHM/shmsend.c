#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/mman.h>
#define  SHM_NAME "/desd_shm"
#define SIZE 4095
int maun()
{
    int shm_fd =shm_open(SHM_NAME,O_CREAT | O_RDWR,0666);
    if(shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }
 if(ftruncate(shm_fd,SIZE)== -1)
 {
    perror("ftruncate");
 exit(1);
 }

 char *shm_ptr =mmap(NULL,SIZE,PROT_READ |  PROT_WRITE,MAP_SHARED,shm_fd,0);
 if(shm_ptr == MAP_FAILED){
    perror("mmap");
    exit(1);
 }

}